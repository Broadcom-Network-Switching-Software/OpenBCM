
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_key.h>
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#endif

#define DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT (0)

#define DNX_FIELD_KEY_FFC_LAYER_OFFSET(_unit, _stage, _offset, _size) \
   ((dnx_data_field.stage.stage_info_get(_unit, _stage)->pbus_header_length > 0) ? \
    (dnx_data_field.stage.stage_info_get(_unit, _stage)->pbus_header_length - (_offset) - (_size)) : \
    (-1))

#define DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID (12)

shr_error_e
dnx_field_key_template_t_init(
    int unit,
    dnx_field_key_template_t * key_template_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    sal_memset(key_template_p, 0x0, sizeof(*key_template_p));

    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        key_template_p->key_qual_map[qual_idx].qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    key_template_p->key_size_in_bits = 0;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_attach_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_attach_info_p, _SHR_E_PARAM, "qual_info");

    sal_memset(qual_attach_info_p, 0x0, sizeof(*qual_attach_info_p));

    qual_attach_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;

    qual_attach_info_p->input_arg = BCM_FIELD_INVALID;
    qual_attach_info_p->offset = BCM_FIELD_INVALID;

    qual_attach_info_p->base_qual = DNX_FIELD_QUAL_TYPE_INVALID;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_attach_info_in_t_init(
    int unit,
    dnx_field_key_attach_info_in_t * key_in_info_p)
{
    int ii = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    sal_memset(key_in_info_p, 0x0, sizeof(*key_in_info_p));

    key_in_info_p->app_db_id = DNX_FIELD_APP_DB_ID_INVALID;
    key_in_info_p->fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    key_in_info_p->field_stage = DNX_FIELD_STAGE_INVALID;
    key_in_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &key_in_info_p->qual_info[ii]));
    }
    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &key_in_info_p->key_template));

    key_in_info_p->compare_id = DNX_FIELD_GROUP_COMPARE_ID_NONE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_attached_qual_info_t_init(
    int unit,
    dnx_field_key_attached_qual_info_t * attached_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(attached_qual_info_p, 0x0, sizeof(*attached_qual_info_p));

    SHR_NULL_CHECK(attached_qual_info_p, _SHR_E_PARAM, "attached_qual_info_p");
    attached_qual_info_p->qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
    attached_qual_info_p->ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
    attached_qual_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;
    attached_qual_info_p->index = BCM_FIELD_INVALID;
    attached_qual_info_p->offset = BCM_FIELD_INVALID;
    attached_qual_info_p->ranges = 0;
    attached_qual_info_p->size = 0;
    attached_qual_info_p->native_pbus = FALSE;
    attached_qual_info_p->offset_on_key = 0;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_sharing_info_t_init(
    int unit,
    dnx_field_key_ffc_sharing_info_t info[DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID])
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(info[0]) * DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID);

    info[DBAL_ENUM_FVAL_FIELD_KEY_A].is_key_id_valid = FALSE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_A].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_A].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_B].is_key_id_valid = FALSE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_B].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_B].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_C].is_key_id_valid = FALSE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_C].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_C].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_D].is_key_id_valid = FALSE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_D].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_D].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_E].is_key_id_valid = FALSE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_E].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_E].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_F].is_key_id_valid = TRUE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_F].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_F].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_G].is_key_id_valid = TRUE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_G].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_G].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_H].is_key_id_valid = TRUE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_H].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_H].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_I].is_key_id_valid = TRUE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_I].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_I].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    info[DBAL_ENUM_FVAL_FIELD_KEY_J].is_key_id_valid = TRUE;
    info[DBAL_ENUM_FVAL_FIELD_KEY_J].offset_start = 0;
    info[DBAL_ENUM_FVAL_FIELD_KEY_J].offset_end = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 ranges,
    int allocate_with_id,
    uint32 *ffc_id)
{
    int allocated_ffc_id = DNX_FIELD_FFC_ID_INVALID;
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (allocate_with_id)
    {
        allocated_ffc_id = (*ffc_id);
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_1_ffc_allocate(unit, alloc_flags, context_id, ranges,
                                                                   &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_2_ffc_allocate(unit, alloc_flags, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_3_ffc_allocate(unit, alloc_flags, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_epmf_ffc_allocate(unit, alloc_flags, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate(unit, alloc_flags, context_id, &allocated_ffc_id));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal stage =%d \n", field_stage);
            break;
        }
    }

    *ffc_id = allocated_ffc_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_single_ffc_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_1_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_2_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_3_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_epmf_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal stage =%d \n", field_stage);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ipmf1_initial_key_occupation_bmp_ffc_deallocate(
    int unit,
    int context_id,
    dnx_field_key_initial_ffc_info_t * ffc_initial_p)
{
    int ffc_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_initial_p, _SHR_E_PARAM, "ffc_initial_p");

    ffc_size = ffc_initial_p->ffc.ffc_instruction.size + 1;

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "de-allocate initial placeholder for context_id %d key_id %d size 0x%X field_stage ipmf1 dest_offset 0x%X\n",
                 context_id, ffc_initial_p->ffc.key_id, ffc_size, ffc_initial_p->key_dest_offset);

    SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                    (unit, context_id, ffc_size, ffc_initial_p->ffc.key_id, ffc_initial_p->key_dest_offset));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_initial_key_to_meta2_qual_get(
    int unit,
    dbal_enum_value_field_field_key_e initial_key_id,
    dnx_field_metadata_qual_e * qual_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_id, _SHR_E_PARAM, "qual_id");

    switch (initial_key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
        {
            *qual_id = DNX_FIELD_QUAL_KEY_F_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
        {
            *qual_id = DNX_FIELD_QUAL_KEY_G_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
        {
            *qual_id = DNX_FIELD_QUAL_KEY_H_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
        {
            *qual_id = DNX_FIELD_QUAL_KEY_I_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
        {
            *qual_id = DNX_FIELD_QUAL_KEY_J_INITIAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal  id =%d \n", initial_key_id);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_action_to_meta2_qual_get(
    int unit,
    dbal_enum_value_field_field_io_e qual_action_id,
    int second_part_payload,
    dnx_field_qual_t * dnx_qual_p)
{
    dnx_field_qual_id_t qual_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_p, _SHR_E_PARAM, "dnx_qual_p");

    switch (qual_action_id)
    {
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1;
            }
            else
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2;
            }
            else
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3;
            }
            else
            {
                qual_id = DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_LEXEM:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_EXEM_ACTION;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0:
        {
            qual_id = DNX_FIELD_QUAL_PMF1_DIRECT_ACTION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Id %d Not supported\n", qual_action_id);
            break;
        }
    }

    *dnx_qual_p = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, qual_id);

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_field_key_ffc_instruction_info_init(
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_p)
{
    ffc_instruction_p->ffc_type = DBAL_FIELD_EMPTY;
    ffc_instruction_p->offset = 0;
    ffc_instruction_p->field_index = 0;
    ffc_instruction_p->key_offset = 0;
    ffc_instruction_p->size = 0;
    ffc_instruction_p->full_instruction = 0;
}

void
dnx_field_key_qualifier_ffc_info_init(
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 num_of_elements)
{
    uint32 qual_idx, jj;

    sal_memset(qualifier_ffc_info_p, 0x0, sizeof(*qualifier_ffc_info_p) * num_of_elements);

    for (qual_idx = 0; qual_idx < num_of_elements; qual_idx++)
    {
        for (jj = 0; jj < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL; jj++)
        {
            qualifier_ffc_info_p[qual_idx].qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.key_dest_offset = 0;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.ffc.key_id = 0;
            dnx_field_key_ffc_instruction_info_init(&(qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc.ffc_instruction));
            dnx_field_key_ffc_instruction_info_init(&
                                                    (qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.
                                                     ffc.ffc_instruction));
        }
    }
}

static shr_error_e
dnx_field_key_ffc_allocate_per_qualifier(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int qual_size,
    uint8 ranges,
    int use_specific_ffc_id,
    int forced_ffc_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_idx = 0;
    int size = qual_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    if (use_specific_ffc_id && (forced_ffc_id == DNX_FIELD_FFC_ID_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Demanded a specific FFC_ID to be allocated, but invalid FFC_ID provided.\n");
    }

    for (ffc_idx = 0; size > 0; ffc_idx++, size -= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        if (ffc_idx >= DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier requires more than %d FFCs.\n", DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL);
        }
        if (use_specific_ffc_id && (ffc_idx > 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Demanded a specific FFC_ID to be allocated, but does not fit in one FFC. "
                         "qual_isze %d.\n", qual_size);
        }
        if (use_specific_ffc_id)
        {
            qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id = forced_ffc_id;
        }
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ffc_allocate(unit, field_stage, context_id, ranges, use_specific_ffc_id,
                                                            &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id)),
                                 "Failed to allocate FFC %d for qualifier \"%s\" for context %d.",
                                 ffc_idx, dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type), context_id);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id[%d] %d, qual size %d, context_id %d\n", ffc_idx,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id, qual_size, context_id);

    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_allocate_initial_ffc(
    int unit,
    int context_id,
    uint8 qual_ffc_ranges,
    uint32 *ffc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_id_p, _SHR_E_PARAM, "ffc_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_allocate(unit, DNX_FIELD_STAGE_IPMF1, context_id, qual_ffc_ranges, FALSE,
                                               ffc_id_p));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id %d, context_id %d, qual_ffc_ranges %d %s\n", *ffc_id_p, context_id,
                 qual_ffc_ranges, EMPTY);

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_field_key_ffc_allocate_per_initial_qualifier(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_qual_t dnx_qual,
    int ffc_index,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_qual_class_e qual_class = DNX_QUAL_CLASS(dnx_qual);
    int qual_size = dnx_qual_info_p->size;
    int qual_size_left = qual_size - (ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC);
    int ffc_size;
    int bit_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    if (ffc_index >= DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier requires more than %d FFCs.\n", DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL);
    }
    ffc_size = ((qual_size_left > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) ?
                DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC : qual_size_left);

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate(unit,
                                                                                          cascaded_from_context_id,
                                                                                          ffc_size, FALSE,
                                                                                          &
                                                                                          (qualifier_ffc_info_p->ffc_info
                                                                                           [ffc_index].ffc_initial.ffc.
                                                                                           key_id), &bit_offset),
                             "Failed to allocate place in initial key for FFC %d for qualifier \"%s\""
                             "for iPMF1 context %d.", ffc_index, dnx_field_dnx_qual_text(unit,
                                                                                         qualifier_ffc_info_p->qual_type),
                             cascaded_from_context_id);
    qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.key_dest_offset = bit_offset;

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_allocate_initial_ffc(unit, cascaded_from_context_id,
                                                                dnx_qual_info_p->ranges,
                                                                &(qualifier_ffc_info_p->ffc_info[ffc_index].
                                                                  ffc_initial.ffc.ffc_id)),
                             "Failed to allocate FFC %d in initial key for qualifier \"%s\" for iPMF! context %d.",
                             ffc_index, dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type),
                             cascaded_from_context_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "allocated initial ffc&key for qual_type %s key_id %d qual_size %d field_stage ipmf1 dest_offset %d\n",
                 dnx_field_dnx_qual_text(unit, dnx_qual),
                 qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.key_id,
                 qual_size, qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_instruction.key_offset);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "initial_key ffc_index %d class %d size %d cascaded_from_context_id %d\n",
                 ffc_index, qual_class, dnx_qual_info_p->size, cascaded_from_context_id);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_hw_table_name_get(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_key_e key_id,
    dbal_tables_e * table_name_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_name_p, _SHR_E_PARAM, "table_name_p");

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            if (key_id <= DBAL_ENUM_FVAL_FIELD_KEY_E)
            {
                *table_name_p = DBAL_TABLE_KLEAP_IPMF1_KBR_INFO;
            }
            else
            {
                *table_name_p = DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO;
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            *table_name_p = DBAL_TABLE_KLEAP_IPMF2_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            *table_name_p = DBAL_TABLE_KLEAP_IPMF3_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            *table_name_p = DBAL_TABLE_KLEAP_E_PMF_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            *table_name_p = DBAL_TABLE_KLEAP_FWD12_KBR_INFO;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Illegal field stage %d \n", field_stage);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_arr_2_bmp(
    int unit,
    uint32 *ffc_arr,
    uint32 *ffc_bitmap)
{
    int ffc_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_arr, _SHR_E_PARAM, "ffc_arr");
    SHR_NULL_CHECK(ffc_bitmap, _SHR_E_PARAM, "ffc_bitmap");

    for (ffc_id = 0;
         ffc_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC && ffc_arr[ffc_id] != DNX_FIELD_FFC_ID_INVALID; ffc_id++)
    {
        DNX_FIELD_ARR32_SET_BIT(ffc_bitmap, (ffc_arr[ffc_id]));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_bmp_2_arr(
    int unit,
    uint32 *ffc_bitmap,
    uint32 *ffc_arr)
{
    int ffc_id;
    int ffc_count = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_bitmap, _SHR_E_PARAM, "ffc_bitmap");
    SHR_NULL_CHECK(ffc_arr, _SHR_E_PARAM, "ffc_arr");

    for (ffc_id = 0; ffc_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_id++)
    {
        if (DNX_FIELD_ARR32_IS_BIT_SET(ffc_bitmap, ffc_id))
        {
            ffc_arr[ffc_count++] = ffc_id;
        }
    }

    for (; ffc_count < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_count++)
    {
        ffc_arr[ffc_count] = DNX_FIELD_FFC_ID_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_ffc_array_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    dbal_fields_e dbal_ctx_id;
    dbal_fields_e dbal_key;
    uint32 ffc_alloc_bmp[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_UINT32] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_id, _SHR_E_PARAM, "ffc_id");

    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_arr_2_bmp(unit, ffc_id, ffc_alloc_bmp));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Set  ffc bitmap context_id %d, key_id %d ffc_alloc_bmp 0x%X%X\n",
                 context_id, key_id, ffc_alloc_bmp[1], ffc_alloc_bmp[0]);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    SHR_IF_ERR_EXIT(dnx_field_map_key_to_field(unit, field_stage, &dbal_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key, key_id);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_alloc_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_kbr_ffc_array_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    dbal_fields_e dbal_ctx_id;
    dbal_fields_e dbal_key;
    uint32 ffc_bmp[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_UINT32] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_id, _SHR_E_PARAM, "ffc_id");

    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    SHR_IF_ERR_EXIT(dnx_field_map_key_to_field(unit, field_stage, &dbal_key));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key, key_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Read ffc bitmap context_id %d, key_id %d ffc_bmp (0x%X,0x%X)\n",
                 context_id, key_id, ffc_bmp[1], ffc_bmp[0]);

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_bmp_2_arr(unit, ffc_bmp, ffc_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_single_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id_new)
{
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 ffc_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (ffc_id_new == DNX_FIELD_FFC_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid FFC ID.\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    for (ffc_index = 0;
         (ffc_index < (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC - 1))
         && (ffc_id[ffc_index] != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {

    }

    if (ffc_id[ffc_index] == DNX_FIELD_FFC_ID_INVALID)
    {

        ffc_id[ffc_index] = ffc_id_new;
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_set(unit, field_stage, context_id, key_id, ffc_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "FFC array is full.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_single_ffc_hw_remove(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id_remove)
{
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 ffc_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    for (ffc_index = 0;
         (ffc_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_index] != ffc_id_remove); ffc_index++);

    if (ffc_index == DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ffc id %d wasn't found in key %d\n", ffc_id_remove, key_id);
    }

    for (; ffc_index < (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC - 1)
         && (ffc_id[ffc_index] != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {

        ffc_id[ffc_index] = ffc_id[ffc_index + 1];
    }

    ffc_id[ffc_index] = DNX_FIELD_FFC_ID_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_set(unit, field_stage, context_id, key_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_qualifier_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    for (ffc_index = 0;
         (ffc_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add
                        (unit, field_stage, context_id, key_id, qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_app_db_id_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    int app_db_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_app_db_id_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    int app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    if (key_id_p->id[0] == DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key is invalid");
    }
    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_set(unit, field_stage, context_id, key_id_p->id[0], app_db_id));

    if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_set
                        (unit, field_stage, context_id, key_id_p->id[1],
                         DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(app_db_id)));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_app_db_id_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *app_db_id_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_info_get_split_const(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_SUB_QUAL],
    unsigned int *nof_sub_qual_p)
{
    uint32 in_const;
    unsigned int is_first_digit_set;
    unsigned int length_sub;
    dnx_field_qual_t all_ones_qual;
    dnx_field_qual_t all_zeros_qual;
    int all_ones_offset;
    uint32 all_ones_size;
    unsigned int sub_position;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(dnx_qual_info_sub_qual, _SHR_E_PARAM, "key_dnx_qual_info_sub_qualqual_map_p");
    SHR_NULL_CHECK(nof_sub_qual_p, _SHR_E_PARAM, "nof_sub_qual_p");

    if (dnx_qual_info_p->size <= 0 || dnx_qual_info_p->size > SAL_UINT32_NOF_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant qualifier size must be between %d and %d. It is %d.\r\n",
                     1, SAL_UINT32_NOF_BITS, dnx_qual_info_p->size);
    }

    in_const = dnx_qual_info_p->index;
    if (in_const != dnx_qual_info_p->index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant in input argument %d was truncated when converted to uint32, "
                     "from %u to %u.\r\n", dnx_qual_info_p->index, dnx_qual_info_p->index, in_const);
    }
    (*nof_sub_qual_p) = 0;

    if ((dnx_qual_info_p->size < SAL_UINT32_NOF_BITS) && ((in_const & SAL_FROM_BIT(dnx_qual_info_p->size)) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant %u doesn't fit inside bit size %d.\r\n", in_const, dnx_qual_info_p->size);
    }

    all_ones_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_QUAL_ALL_ONES);
    all_ones_offset = 0;
    all_ones_size = 0;
    if (in_const != 0)
    {
        int nof_mappings;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, all_ones_qual, &nof_mappings, NULL, &all_ones_size));
        if (all_ones_size == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage \"%s\" does not support non-zero constant qualifiers. Input argument for const "
                         "qualifier 0x%x (\"%s\") is 0x%x.\r\n",
                         dnx_field_stage_text(unit, field_stage),
                         dnx_qual_info_p->qual_type, dnx_field_dnx_qual_text(unit, dnx_qual_info_p->qual_type),
                         dnx_qual_info_p->index);
        }
        {
            int offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, all_ones_qual, &nof_mappings, offsets));

            if (nof_mappings != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "All ones qulaifier has split mapping for offsets (nof_mappings %d).\r\n",
                             nof_mappings);
            }
            all_ones_offset = offsets[0];

            if (nof_mappings != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "All ones qulaifier has split mapping for sizes (nof_mappings %d).\r\n",
                             nof_mappings);
            }
        }
    }

    all_zeros_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_SW, field_stage, DNX_FIELD_SW_QUAL_ALL_ZERO);

    sub_position = 0;
    while (sub_position < dnx_qual_info_p->size)
    {

        if (*nof_sub_qual_p >= DNX_FIELD_KEY_MAX_SUB_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Number of sub qualifiers reached maximum (%d) and still  %d bits left in "
                         "const (out of %d).\r\n",
                         DNX_FIELD_KEY_MAX_SUB_QUAL, dnx_qual_info_p->size - sub_position, dnx_qual_info_p->size);
        }
        is_first_digit_set = DNX_FIELD_UINT_IS_BIT_SET(in_const, sub_position);

        for (length_sub = 0;
             (sub_position + length_sub < dnx_qual_info_p->size)
             && (DNX_FIELD_UINT_IS_BIT_SET(in_const, sub_position + length_sub) == is_first_digit_set); length_sub++)
        {

            if (is_first_digit_set && length_sub >= all_ones_size)
            {
                break;
            }
        }

        if (length_sub <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Loop did not progress, consecutive number of zeros or ones less than one (%d).\r\n,",
                         length_sub);
        }

        sal_memcpy(&(dnx_qual_info_sub_qual[*nof_sub_qual_p]), dnx_qual_info_p, sizeof(dnx_qual_info_sub_qual[0]));

        dnx_qual_info_sub_qual[*nof_sub_qual_p].size = length_sub;
        dnx_qual_info_sub_qual[*nof_sub_qual_p].offset_on_key += sub_position;
        if (is_first_digit_set)
        {
            dnx_qual_info_sub_qual[*nof_sub_qual_p].qual_type = all_ones_qual;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].ffc_type = DNX_FIELD_FFC_LITERALLY;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].offset = all_ones_offset;
        }
        else
        {
            dnx_qual_info_sub_qual[*nof_sub_qual_p].qual_type = all_zeros_qual;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].ffc_type = DNX_FIELD_FFC_TYPE_INVALID;

            dnx_qual_info_sub_qual[*nof_sub_qual_p].offset = 0;
        }

        dnx_qual_info_sub_qual[*nof_sub_qual_p].index = 0;

        (*nof_sub_qual_p)++;
        sub_position += length_sub;
    }

    if (sub_position != dnx_qual_info_p->size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%d bits were read out of %d.\r\n", sub_position, dnx_qual_info_p->size);
    }

    if ((in_const == 0) && (*nof_sub_qual_p != 1)
        && (dnx_qual_info_sub_qual[*nof_sub_qual_p].qual_type != all_zeros_qual))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ones were written for a zero const.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_info_get_cascaded_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_attach_info_p)
{
    dnx_field_action_length_type_e action_length_type;
    dnx_field_stage_e cascaded_field_stage;
    int min_offset = -1;
    int max_offset = -1;
    int intermediate_point = -1;
    uint32 qual_size;
    int nof_mappings;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_attach_info_p, _SHR_E_PARAM, "qual_attach_info_p");

    if (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be reached for input type cascaded. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage));
    }

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from other PMF stages only supported in iPMF2. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, qual_attach_info_p->input_arg, &cascaded_field_stage));
    if (cascaded_field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from PMF stages can only cascade from stage iPMF1. "
                     "(input type %d, input argument (field group) %d, "
                     "qualifier %s, cascaded field group stage %s).\r\n",
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, qual_attach_info_p->input_arg, &action_length_type));

    switch (action_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_EXEM:
        case DNX_FIELD_ACTION_LENGTH_TYPE_ACE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_MDB_DT:
        {
            unsigned int block_size;
            min_offset = 0;
            SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(unit, cascaded_field_stage, action_length_type,
                                                                      &block_size));
            max_offset = block_size;
            if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE)
            {
                intermediate_point = dnx_data_field.tcam.action_size_single_get(unit);
            }
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Direct extraction not supported for cascading, "
                         "as it is not supported on iPMF1. "
                         "(input type %d, input argument (field group) %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
        default:
        {
            dnx_field_group_type_e fg_type;
            SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, qual_attach_info_p->input_arg, &fg_type));
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown action_length_type %d. "
                         "(input type %d, input argument (field group) %d, fg_type %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         action_length_type,
                         qual_attach_info_p->input_type, qual_attach_info_p->input_arg, fg_type,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &nof_mappings, NULL, &qual_size));
    if (nof_mappings != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %s has %d mappings, not valid for cascaded input type.\r\n",
                     dnx_field_dnx_qual_text(unit, qual_type), nof_mappings);
    }
    if (qual_attach_info_p->offset < min_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) cannot be smaller then (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_attach_info_p->offset, min_offset, qual_attach_info_p->input_type,
                     qual_attach_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }
    if (qual_attach_info_p->offset + qual_size > max_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) plus qualifier size (%d) (together %d) cannot exceed (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_attach_info_p->offset, qual_size, qual_attach_info_p->offset + qual_size, max_offset,
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }

    if (qual_attach_info_p->offset < intermediate_point && intermediate_point < qual_attach_info_p->offset + qual_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascading qualifier for double key result can't span both "
                     "key payloads. Qualifier start: %d,  end: %d. intermediate point: %d (input type %d,"
                     "input argument (field group) %d, qualifier %s.\r\n",
                     qual_attach_info_p->offset, qual_attach_info_p->offset + qual_size, intermediate_point,
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    int offset_on_key,
    dnx_field_qual_attach_info_t * qual_attach_info_p,
    int *nof_sub_quals_p,
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL])
{
    dnx_field_qual_class_e dnx_qual_class;
    int nof_mappings;
    uint32 sizes[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    uint32 size_total;
    uint32 sizes_sum;
    uint32 qual_size;
    int sub_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_attach_info_p, _SHR_E_PARAM, "qual_attach_info_p");
    SHR_NULL_CHECK(nof_sub_quals_p, _SHR_E_PARAM, "nof_sub_quals_p");
    SHR_NULL_CHECK(dnx_qual_info, _SHR_E_PARAM, "dnx_qual_info");

    dnx_qual_class = DNX_QUAL_CLASS(qual_type);

    switch (qual_attach_info_p->input_type)
    {
        case DNX_FIELD_INPUT_TYPE_LAYER_FWD:

        case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The header is not accessible to iPMF3, and thus layer (header) "
                             "input types cannot be used in that stage (qualifier %s).\r\n",
                             dnx_field_dnx_qual_text(unit, qual_type));
            }
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_HEADER) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied input type (header) for qual %s does not match with the corresponding qualifier class(%s). \n",
                             dnx_field_dnx_qual_text(unit, qual_type), dnx_field_qual_class_text(dnx_qual_class));

            }
            if (qual_attach_info_p->input_arg >= DNX_FIELD_LAYER_NOF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied input arg(%d) for qual %s is out of range for input type LAYER. \n",
                             qual_attach_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (qual_attach_info_p->offset >
                dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length
                || qual_attach_info_p->offset < -dnx_data_field.stage.stage_info_get(unit,
                                                                                     field_stage)->pbus_header_length)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) for qual %s is out of range (%d:%d) "
                             "for input type LAYER for stage %s. \n",
                             qual_attach_info_p->offset, dnx_field_dnx_qual_text(unit, qual_type),
                             -dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                             dnx_field_stage_text(unit, field_stage));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER)
                && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_VW))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type metadata can only support metadata, user defined "
                             "qualifiers of virtual wire qualifiers (qualifier %s).\r\n",
                             dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER && qual_attach_info_p->offset < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) for user defined qual %s is negative. "
                             "Offset may not be negative for user defined qualifier with input type metadata.\n",
                             qual_attach_info_p->offset, dnx_field_dnx_qual_text(unit, qual_type));
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, NULL, NULL, &qual_size));
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER && (qual_attach_info_p->offset + qual_size) >
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                             "stage %s (%d).\n",
                             qual_attach_info_p->offset, qual_size, qual_attach_info_p->offset + qual_size,
                             dnx_field_stage_text(unit, field_stage),
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus);
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA2:
        {
            if (field_stage != DNX_FIELD_STAGE_IPMF2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Only stage iPMF2 support input type metadata2 (%d), stage used: %s (%d). "
                             "Qualifier %s.\r\n",
                             DNX_FIELD_INPUT_TYPE_META_DATA2, dnx_field_stage_text(unit, field_stage), field_stage,
                             dnx_field_dnx_qual_text(unit, qual_type));
            }

            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type secondary metadata can only support metadata2 or "
                             "user defined qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER && qual_attach_info_p->offset < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) for user defined qual %s is negative. "
                             "Offset may not be negative for user defined qualifier with input type metadata.\n",
                             qual_attach_info_p->offset, dnx_field_dnx_qual_text(unit, qual_type));
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, NULL, NULL, &qual_size));
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER && (qual_attach_info_p->offset + qual_size) >
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                             "stage %s (%d).\n",
                             qual_attach_info_p->offset, qual_size, qual_attach_info_p->offset + qual_size,
                             dnx_field_stage_text(unit, field_stage),
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus);
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:

        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_LAYER_RECORD) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type layer record can only support layer record or user defined "
                             "qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (qual_attach_info_p->input_arg >= DNX_FIELD_LAYER_NOF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied input arg(%d) is out of range for input type LAYER_RECORDS. \n",
                             qual_attach_info_p->input_arg);
            }
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER &&
                (qual_attach_info_p->offset > DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS ||
                 qual_attach_info_p->offset < 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) for qual %s is out of range for input type LAYER_RECORDS (%d:%d). \n",
                             qual_attach_info_p->offset, dnx_field_dnx_qual_text(unit, qual_type),
                             0, DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS);
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CONST:
        {
            if (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type const can only support software or user defined "
                             "qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_EXTERNAL:
        {
            if (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type external can only support user defined qualifiers. "
                             "Got qual class %s (qualifier %s).\r\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER && qual_attach_info_p->offset < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied offset(%d) for user defined qual %s is negative. "
                             "Offset may not be negative for user defined qualifier with input type KBP.\n",
                             qual_attach_info_p->offset, dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CASCADED:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type cascade can only support user defined qualifiers or meta2. "
                             "Got qual class %s (qualifier %s).\r\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_dnx_qual_text(unit, qual_type));
            }
            if (qual_attach_info_p->input_arg >= dnx_data_field.group.nof_fgs_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The supplied input arg(%d) for qual %s is out of range for input type CASCADED - must be valid FG ID. \n",
                             qual_attach_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Input type %d for qualifier %s is not supported.\r\n",
                         qual_attach_info_p->input_type, dnx_field_dnx_qual_text(unit, qual_type));
        }
            break;
    }

    if ((qual_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE)
        || (qual_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE))
    {
        if (qual_attach_info_p->input_arg < 0)
            SHR_ERR_EXIT(_SHR_E_PARAM, "Absolute input types cannot take negative layer index "
                         "(input type %d, input argument %d, qualifier %s).\r\n",
                         qual_attach_info_p->input_type, qual_attach_info_p->input_arg, dnx_field_dnx_qual_text(unit,
                                                                                                                qual_type));
    }

    if (qual_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_cascaded_verify(unit, field_stage, qual_type, qual_attach_info_p));
    }

    if (qual_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_EXTERNAL)
    {
        switch (dnx_data_field.external_tcam.type_get(unit))
        {
            case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
            {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
                SHR_IF_ERR_EXIT(dnx_field_kbp_key_qual_info_get_verify
                                (unit, field_stage, qual_type, qual_attach_info_p));
#else
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\n");
#endif
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown external TCAM device %d.\r\n",
                             dnx_data_field.external_tcam.type_get(unit));
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &nof_mappings, sizes, &size_total));

    if (nof_mappings <= 0 || nof_mappings > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier has %d mappings, must be between (1-%d).\n",
                     nof_mappings, DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
    }
    sizes_sum = 0;
    for (sub_ndx = 0; sub_ndx < nof_mappings; sub_ndx++)
    {
        sizes_sum += sizes[sub_ndx];
    }
    if (sizes_sum != size_total)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier \"%s\" total size %d, but sum of parts %d.\r\n",
                     dnx_field_dnx_qual_text(unit, qual_type), size_total, sizes_sum);
    }
    if ((nof_mappings != 1) && (qual_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_CONST))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Qualifier \"%s\" broken into %d parts in mapping, not supported for const input type.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), nof_mappings);
    }
    if ((nof_mappings != 1) && (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER))
    {

        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Qualifier \"%s\" broken into %d parts in mapping, not supported for user defined qualifers.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), nof_mappings);
    }
    if ((nof_mappings != 1) && (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_META_DATA) &&
        (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_META_DATA2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Qualifier \"%s\" broken into %d parts in mapping, not supported for non metadata qualifiers.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), nof_mappings);
    }

    if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER) && (qual_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Qualifier \"%s\" (0x%x) is not user defined but has base qualifier 0x%x.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual);
    }
    if (qual_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID)
    {
        int base_qual_nof_mappings;
        uint32 base_qual_size_total;
        if (DNX_QUAL_STAGE(qual_attach_info_p->base_qual) != field_stage)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon 0x%x with stage %d instead of %d.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual,
                         DNX_QUAL_STAGE(qual_attach_info_p->base_qual), field_stage);
        }
        if ((DNX_QUAL_CLASS(qual_attach_info_p->base_qual) != DNX_FIELD_QUAL_CLASS_META) &&
            (DNX_QUAL_CLASS(qual_attach_info_p->base_qual) != DNX_FIELD_QUAL_CLASS_META2))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon 0x%x, but it is not a metadata qualifeir.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual);
        }
        if ((DNX_QUAL_CLASS(qual_attach_info_p->base_qual) == DNX_FIELD_QUAL_CLASS_META) &&
            (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_META_DATA))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon 0x%x, "
                         "a non native metadata class but input type isn't non native metadata.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual);
        }
        if ((DNX_QUAL_CLASS(qual_attach_info_p->base_qual) == DNX_FIELD_QUAL_CLASS_META2) &&
            (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_META_DATA2))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon 0x%x, "
                         "a native metadata class but input type isn't native metadata.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, qual_attach_info_p->base_qual, &base_qual_nof_mappings, NULL,
                         &base_qual_size_total));

        if (nof_mappings <= 0 || nof_mappings > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s (0x%x) is based upon 0x%x, which has %d mappings, "
                         "must be between (1-%d).\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual,
                         nof_mappings, DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
        }
        if (qual_attach_info_p->offset < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon 0x%x, "
                         "and has negative offset %d. A qualifier based upon another cannot exceed "
                         "the boundaries of the base qualifier.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type, qual_attach_info_p->base_qual,
                         qual_attach_info_p->offset);
        }
        if (qual_attach_info_p->offset + size_total > base_qual_size_total)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" (0x%x) is based upon \"%s\" 0x%x (with size %d), "
                         "and has offset %d and size %d, together %d. A qualifier based upon another cannot exceed "
                         "the boundaries of the base qualifier.\n",
                         dnx_field_dnx_qual_text(unit, qual_type), qual_type,
                         dnx_field_dnx_qual_text(unit, qual_attach_info_p->base_qual), qual_attach_info_p->base_qual,
                         base_qual_size_total, qual_attach_info_p->offset, size_total,
                         qual_attach_info_p->offset + size_total);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ipmf2_base_cascaded_qual_get(
    int unit,
    int context_id,
    dnx_field_group_t cascaded_from_fg_id,
    int relative_qual_offset,
    dnx_field_qual_t * ipmf2_base_qual_p)
{
    dnx_field_context_t cascaded_from_context_id;
    dbal_enum_value_field_field_io_e ipfm1_cascaded_from_field_io;
    int second_part_payload;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_base_qual_p, _SHR_E_PARAM, "ipmf2_base_qual_p");

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    SHR_IF_ERR_EXIT(dnx_field_group_field_io_get
                    (unit, cascaded_from_fg_id, cascaded_from_context_id, &ipfm1_cascaded_from_field_io));

    second_part_payload = (relative_qual_offset < dnx_data_field.tcam.action_size_single_get(unit)) ? (FALSE) : (TRUE);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_action_to_meta2_qual_get
                    (unit, ipfm1_cascaded_from_field_io, second_part_payload, ipmf2_base_qual_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_prepare_cascaded_qual_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t cascaded_from_fg_id,
    int relative_qual_offset,
    int *absolute_qual_offset,
    int *native_pbus_p)
{
    dnx_field_qual_t internal_ipmf2_qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_qual_attach_info_t internal_qual_attach_info;
    dnx_field_stage_e cascaded_from_field_stage;
    int nof_mappings;
    int qual_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    int action_buffer_offset;
    int template_shift;
    dnx_field_group_type_e fg_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(absolute_qual_offset, _SHR_E_PARAM, "absolute_qual_offset");
    SHR_NULL_CHECK(native_pbus_p, _SHR_E_PARAM, "native_pbus_p");

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, cascaded_from_fg_id, &cascaded_from_field_stage));
    if (field_stage != DNX_FIELD_STAGE_IPMF2 || cascaded_from_field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded from field group %d is in stage \"%s\" to stage \"%s\". "
                     "Only cascading from iPMF1 to iPMF2 supported.\r\n",
                     cascaded_from_fg_id,
                     dnx_field_stage_text(unit, cascaded_from_field_stage), dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &internal_qual_attach_info));
    internal_qual_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_base_cascaded_qual_get
                    (unit, context_id, cascaded_from_fg_id, relative_qual_offset, &internal_ipmf2_qual_type));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &nof_mappings, qual_offsets));
    action_buffer_offset = qual_offsets[0];

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, cascaded_from_fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM
        && relative_qual_offset >= dnx_data_field.tcam.action_size_single_get(unit))
    {
        relative_qual_offset -= dnx_data_field.tcam.action_size_single_get(unit);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, cascaded_from_fg_id, &template_shift));

    *absolute_qual_offset = action_buffer_offset + relative_qual_offset + template_shift;

    (*native_pbus_p) = TRUE;

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Qualifier for action %s context %d BASE internal_ipmf2_qual_info.offset 0x%X BASE "
                 "mapped_offset 0x%X\n",
                 dnx_field_dnx_qual_text(unit, internal_ipmf2_qual_type), context_id,
                 internal_qual_attach_info.offset, *absolute_qual_offset);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_qual_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_t qual_type,
    int offset_on_key,
    dnx_field_qual_attach_info_t * qual_attach_info_p,
    int *nof_sub_quals_p,
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL])
{
    int index;
    int layer_record_offset;
    int base_offset;
    int added_offset;
    int nof_mappings_offset;
    int nof_mappings_size;
    int mapped_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    uint32 sizes[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    uint32 size_total;
    int sub_ndx;
    int nof_mappings;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_key_qual_info_get_verify
                           (unit, field_stage, qual_type, offset_on_key, qual_attach_info_p,
                            nof_sub_quals_p, dnx_qual_info));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, qual_type, &nof_mappings_offset, mapped_offsets));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &nof_mappings_size, sizes, &size_total));

    if (nof_mappings_offset != nof_mappings_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch. Qualifier %s (0x%x) is mapped to nof_mappings %d and %d.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), qual_type, nof_mappings_offset, nof_mappings_size);
    }

    nof_mappings = nof_mappings_size;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s mapped to %d pieces.%s\n",
                 dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                 nof_mappings, EMPTY);

    if (qual_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID)
    {
        int base_qual_nof_mappings_offset;
        int base_qual_nof_mappings_size;
        int base_qual_mapped_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
        uint32 base_qual_sizes[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
        uint32 base_qual_size_total;
        int remaining_size = size_total;
        int remaining_offset = qual_attach_info_p->offset;
        int nof_mappings_used;
        int base_qual_sub_ndx;

        if (nof_mappings != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier has base qualifier and multiple mapping in the same time.\n");
        }
        if (mapped_offsets[0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier has base qualifier and non zero mapped offset %d.\n",
                         mapped_offsets[0]);
        }
        if (qual_attach_info_p->offset < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier has base qualifier and negative offset in attach info %d.\n",
                         qual_attach_info_p->offset);
        }

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                        (unit, field_stage, qual_attach_info_p->base_qual, &base_qual_nof_mappings_offset,
                         base_qual_mapped_offsets));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, qual_attach_info_p->base_qual, &base_qual_nof_mappings_size,
                         base_qual_sizes, &base_qual_size_total));

        if (nof_mappings_offset != nof_mappings_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch. base_qual %s (0x%x) is mapped to nof_mappings %d and %d.\n",
                         dnx_field_dnx_qual_text(unit, qual_attach_info_p->base_qual), qual_attach_info_p->base_qual,
                         base_qual_nof_mappings_offset, base_qual_nof_mappings_size);
        }
        if ((nof_mappings_size < 1) || (nof_mappings_size > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "base_qual %s (0x%x) has %d mappings, must be 1-%d.\n",
                         dnx_field_dnx_qual_text(unit, qual_attach_info_p->base_qual), qual_attach_info_p->base_qual,
                         nof_mappings_size, DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
        }

        for (base_qual_sub_ndx = 0; base_qual_sub_ndx < base_qual_nof_mappings_size; base_qual_sub_ndx++)
        {
            if (base_qual_sizes[base_qual_sub_ndx] <= remaining_offset)
            {
                remaining_offset -= base_qual_sizes[base_qual_sub_ndx];
            }
            else
            {
                break;
            }
        }

        if (base_qual_sub_ndx > base_qual_nof_mappings_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached illogical point.\n");
        }

        if (base_qual_sub_ndx == base_qual_nof_mappings_size)
        {
            base_qual_sub_ndx--;
            remaining_offset += base_qual_sizes[base_qual_sub_ndx];
            sizes[0] = remaining_size;
            mapped_offsets[0] = remaining_offset + base_qual_mapped_offsets[base_qual_sub_ndx];
            nof_mappings_used = 1;
        }
        else
        {

            mapped_offsets[0] = remaining_offset + base_qual_mapped_offsets[base_qual_sub_ndx];
            if (remaining_size > (base_qual_sizes[base_qual_sub_ndx] - remaining_offset))
            {
                sizes[0] = base_qual_sizes[base_qual_sub_ndx] - remaining_offset;
                remaining_size -= base_qual_sizes[base_qual_sub_ndx] - remaining_offset;
            }
            else
            {
                sizes[0] = remaining_size;
                remaining_size = 0;
            }
            nof_mappings_used = 1;

            while (remaining_size > 0)
            {
                base_qual_sub_ndx++;

                if (base_qual_sub_ndx == base_qual_nof_mappings_size)
                {
                    sizes[base_qual_sub_ndx - 1] += remaining_size;
                    remaining_size = 0;
                    break;
                }

                mapped_offsets[nof_mappings_used] = base_qual_mapped_offsets[base_qual_sub_ndx];
                if (remaining_size > base_qual_sizes[base_qual_sub_ndx])
                {
                    sizes[nof_mappings_used] = base_qual_sizes[base_qual_sub_ndx];
                    remaining_size -= base_qual_sizes[base_qual_sub_ndx];
                }
                else
                {
                    sizes[nof_mappings_used] = remaining_size;
                    remaining_size = 0;
                }
                nof_mappings_used++;
            }
            nof_mappings = nof_mappings_used;
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s based_qual %s mapped to %d pieces.%s\n",
                     dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_dnx_qual_text(unit, qual_attach_info_p->base_qual), nof_mappings, EMPTY);
    }

    for (sub_ndx = 0; sub_ndx < nof_mappings; sub_ndx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_attached_qual_info_t_init(unit, &(dnx_qual_info[sub_ndx])));

        if (qual_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID)
        {

            base_offset = mapped_offsets[sub_ndx];
            added_offset = 0;
        }
        else if (DNX_QUAL_CLASS(qual_type) == DNX_FIELD_QUAL_CLASS_USER)
        {

            base_offset = mapped_offsets[sub_ndx] + qual_attach_info_p->offset;
            added_offset = 0;
        }
        else
        {

            base_offset = mapped_offsets[sub_ndx];
            added_offset = qual_attach_info_p->offset;
        }

        dnx_qual_info[sub_ndx].input_type = qual_attach_info_p->input_type;

        dnx_qual_info[sub_ndx].size = sizes[sub_ndx];

        dnx_qual_info[sub_ndx].qual_type = qual_type;

        if (sub_ndx > 0)
        {
            dnx_qual_info[sub_ndx].offset_on_key =
                dnx_qual_info[sub_ndx - 1].offset_on_key + dnx_qual_info[sub_ndx - 1].size;
        }
        else
        {
            dnx_qual_info[0].offset_on_key = offset_on_key;
        }

        switch (qual_attach_info_p->input_type)
        {
            case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_RELATIVE_HEADER;
                SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo
                                (unit, field_stage, qual_attach_info_p->input_arg, &index));
                dnx_qual_info[sub_ndx].index = index;
                dnx_qual_info[sub_ndx].offset =
                    DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset,
                                                   dnx_qual_info[sub_ndx].size);
                break;
            }
            case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_ABSOLUTE_HEADER;
                SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo
                                (unit, field_stage, qual_attach_info_p->input_arg, &index));
                dnx_qual_info[sub_ndx].index = index;
                dnx_qual_info[sub_ndx].offset =
                    DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset,
                                                   dnx_qual_info[sub_ndx].size);
                break;
            }
            case DNX_FIELD_INPUT_TYPE_META_DATA:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LITERALLY;

                dnx_qual_info[sub_ndx].index = 0;
                dnx_qual_info[sub_ndx].offset = base_offset;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_META_DATA2:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LITERALLY;

                dnx_qual_info[sub_ndx].index = 0;
                dnx_qual_info[sub_ndx].offset = base_offset;
                dnx_qual_info[sub_ndx].native_pbus = TRUE;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LAYER_RECORD;
                SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo
                                (unit, field_stage, qual_attach_info_p->input_arg, &index));
                dnx_qual_info[sub_ndx].index = index;
                dnx_qual_info[sub_ndx].offset = base_offset;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
            {
                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LITERALLY;
                SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo
                                (unit, field_stage, qual_attach_info_p->input_arg, &index));
                dnx_qual_info[sub_ndx].index = 0;
                SHR_IF_ERR_EXIT(dnx_field_map_layer_record_offset(unit, field_stage, index, &layer_record_offset));
                dnx_qual_info[sub_ndx].offset = layer_record_offset + base_offset;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_EXTERNAL:
            {

                int external_nof_mappings;
                int external_payload_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
                dnx_field_qual_t dnx_qual_external_result;

                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LITERALLY;
                dnx_qual_info[sub_ndx].index = 0;

                dnx_qual_external_result =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0);
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, dnx_qual_external_result,
                                                              &external_nof_mappings, external_payload_offsets));
                dnx_qual_info[sub_ndx].offset = external_payload_offsets[0] + base_offset;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_CASCADED:
            {
                int absolute_qual_offset = 0;

                SHR_IF_ERR_EXIT(dnx_field_key_prepare_cascaded_qual_info(unit, field_stage, context_id,
                                                                         qual_attach_info_p->input_arg,
                                                                         qual_attach_info_p->offset,
                                                                         &absolute_qual_offset,
                                                                         &(dnx_qual_info[sub_ndx].native_pbus)));

                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_LITERALLY;

                dnx_qual_info[sub_ndx].index = qual_attach_info_p->input_arg;

                dnx_qual_info[sub_ndx].offset = absolute_qual_offset;
                break;
            }
            case DNX_FIELD_INPUT_TYPE_CONST:
            {

                dnx_qual_info[sub_ndx].ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
                dnx_qual_info[sub_ndx].index = qual_attach_info_p->input_arg;
                dnx_qual_info[sub_ndx].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported input type: %d.\r\n", qual_attach_info_p->input_type);
            }
        }

        if (dnx_qual_info[sub_ndx].offset < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated offset (%d) is negative.\r\n", dnx_qual_info[sub_ndx].offset);
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s offset %d original offset %d.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                     dnx_qual_info[sub_ndx].offset, qual_attach_info_p->offset);

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s qual_info_p->input_type %d size %d.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                     qual_attach_info_p->input_type, dnx_qual_info[sub_ndx].size);

        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_to_ranges
                        (unit, field_stage, qual_attach_info_p->input_type, dnx_qual_info[sub_ndx].offset,
                         &(dnx_qual_info[sub_ndx].ranges)));
    }

    (*nof_sub_quals_p) = nof_mappings;

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)

shr_error_e
dnx_field_key_qual_info_uses_up_to_one_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_attach_info_p,
    uint8 *is_up_to_one_ffc_p)
{
    int offset_on_key;
    int nof_sub_quals;
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];

    SHR_FUNC_INIT_VARS(unit);

    offset_on_key = 0;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, field_stage, DNX_FIELD_CONTEXT_ID_INVALID, qual_type,
                     offset_on_key, qual_attach_info_p, &nof_sub_quals, dnx_qual_info));

    if (nof_sub_quals <= 0 || nof_sub_quals > DNX_FIELD_KEY_MAX_SUB_QUAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s split into %d parts, must be 1-%d.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), nof_sub_quals, DNX_FIELD_KEY_MAX_SUB_QUAL);
    }
    else if (nof_sub_quals != 1)
    {
        (*is_up_to_one_ffc_p) = FALSE;
    }
    else
    {
        (*is_up_to_one_ffc_p) = TRUE;

        if (dnx_qual_info[0].input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {
            dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_SUB_QUAL];
            unsigned int nof_const_sub_qual;

            SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_split_const(unit, field_stage, &(dnx_qual_info[0]),
                                                                    dnx_qual_info_sub_qual, &nof_const_sub_qual));
            if (nof_const_sub_qual > 1)
            {
                (*is_up_to_one_ffc_p) = FALSE;
            }
        }

        if (dnx_qual_info[0].size > dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit))
        {
            (*is_up_to_one_ffc_p) = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

static shr_error_e
dnx_field_key_calc_ffc_instruction_values(
    int unit,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    uint32 ffc_index,
    uint32 is_read_from_initial,
    uint32 is_initial,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_p)
{
    int qual_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(ffc_instruction_p, _SHR_E_PARAM, "ffc_instruction_p");

    qual_size = dnx_qual_info_p->size;

    if (is_read_from_initial)
    {

        ffc_instruction_p->offset = dnx_qual_info_p->offset;
    }
    else
    {
        ffc_instruction_p->offset =
            dnx_qual_info_p->offset + ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC;
    }

    ffc_instruction_p->field_index = dnx_qual_info_p->index;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "*ffc_offset_p 0x%X, ffc_index %d, dnx_qual_info_p->offset 0x%X field_index %d\n",
                 ffc_instruction_p->offset, ffc_index, dnx_qual_info_p->offset, ffc_instruction_p->field_index);

    if (is_initial)
    {

        ffc_instruction_p->key_offset = (dnx_qual_info_p->offset_on_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    }
    else
    {

        ffc_instruction_p->key_offset =
            (dnx_qual_info_p->offset_on_key +
             ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) %
            DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    }

    if ((qual_size - ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) >=
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        ffc_instruction_p->size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC - 1;
    }
    else
    {
        ffc_instruction_p->size = qual_size - ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC - 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_single_ffc_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_ffc_hw_info_t * ffc_p,
    dnx_field_ffc_type_e ffc_type)
{
    dbal_fields_e dbal_ffc_type;
    dbal_fields_e dbal_ctx_id;
    uint32 ffc_instruction_buffer = 0;
    uint32 entry_handle_id = 0;
    dbal_tables_e table_name;
    uint32 ffc_offset_uint32;
    uint32 ffc_field_index_uint32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_p, _SHR_E_PARAM, "ffc_p");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ffc_type_to_field(unit, field_stage, ffc_type, &dbal_ffc_type));
    if (field_stage != DNX_FIELD_STAGE_L4_OPS)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    }
    if (dbal_ffc_type == DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No ffc type %d in stage %s.\r\n", ffc_type,
                     dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    if (field_stage != DNX_FIELD_STAGE_L4_OPS)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_p->ffc_id);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_OPS_FFC_IDX, ffc_p->ffc_id);
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id %d, context ID %d stage %s %s\n", ffc_p->ffc_id,
                 context_id, dnx_field_stage_text(unit, field_stage), EMPTY);

    ffc_offset_uint32 = ffc_p->ffc_instruction.offset;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_OFFSET, &ffc_offset_uint32, &ffc_instruction_buffer));

    if ((dbal_ffc_type != DBAL_FIELD_IPMF1_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_IPMF2_FFC_LITERALLY) &&
        (dbal_ffc_type != DBAL_FIELD_IPMF3_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_EPMF_FFC_LITERALLY) &&
        (dbal_ffc_type != DBAL_FIELD_FWD12_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_L4OPS_FFC_LITERALLY))
    {

        ffc_field_index_uint32 = ffc_p->ffc_instruction.field_index;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, dbal_ffc_type, DBAL_FIELD_FFC_INDEX, &ffc_field_index_uint32, &ffc_instruction_buffer));
    }

    if (field_stage != DNX_FIELD_STAGE_L4_OPS)
    {

        LOG_DEBUG_EX(BSL_LOG_MODULE, "offset in key 0x%X ffc_offset 0x%X index %d  offset %d\n",
                     ffc_p->ffc_instruction.key_offset, ffc_p->ffc_instruction.offset,
                     ffc_p->ffc_instruction.field_index, ffc_p->ffc_instruction.offset);
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, dbal_ffc_type, DBAL_FIELD_FFC_KEY_OFFSET, &ffc_p->ffc_instruction.key_offset,
                         &ffc_instruction_buffer));
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_SIZE, &ffc_p->ffc_instruction.size,
                     &ffc_instruction_buffer));

    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_ffc_type, INST_SINGLE, ffc_instruction_buffer);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_name, entry_handle_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ipmf2_convert_initial_key_id_into_meta2_qual(
    int unit,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    int offset_on_key,
    dnx_field_key_attached_qual_info_t * internal_ipmf2_qual_info_p)
{
    dnx_field_qual_t internal_ipmf2_dnx_qual;
    dnx_field_metadata_qual_e ipmf2_base_qual = DNX_FIELD_QUAL_ID_INVALID;
    dnx_field_qual_attach_info_t qual_attach_info;
    int nof_mappings;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &qual_attach_info));
    qual_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, key_id, &ipmf2_base_qual));

    internal_ipmf2_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, internal_ipmf2_dnx_qual, offset_on_key,
                     &qual_attach_info, &nof_mappings, internal_ipmf2_qual_info_p));

    if (nof_mappings != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal qualifier to collect initial key to iPMF2 is split to %d parts.\r\n",
                     nof_mappings);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "context %d initial key %s BASE internal_ipmf2_qual_info.offset 0x%X %s\n",
                 context_id, dnx_field_dnx_qual_text(unit, internal_ipmf2_dnx_qual), internal_ipmf2_qual_info_p->offset,
                 EMPTY);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ipmf2_prepare_internal_qual_info(
    int unit,
    int context_id,
    uint8 size,
    int offset_on_key,
    dnx_field_key_initial_ffc_info_t * initial_ffc_info_p,
    dnx_field_key_attached_qual_info_t * internal_ipmf2_qual_info_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_ffc_info_p, _SHR_E_PARAM, "initial_ffc_info_p");
    SHR_NULL_CHECK(internal_ipmf2_qual_info_p, _SHR_E_PARAM, "internal_ipmf2_qual_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_convert_initial_key_id_into_meta2_qual
                    (unit, context_id, initial_ffc_info_p->ffc.key_id, offset_on_key, internal_ipmf2_qual_info_p));

    internal_ipmf2_qual_info_p->offset += initial_ffc_info_p->key_dest_offset;

    internal_ipmf2_qual_info_p->size = size;

    internal_ipmf2_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ipmf2_ffc_set(
    int unit,
    int context_id,
    int ffc_index,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_key_attached_qual_info_t internal_ipmf2_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    dnx_field_key_attached_qual_info_t_init(unit, &internal_ipmf2_qual_info);

    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_prepare_internal_qual_info(unit,
                                                                   context_id,
                                                                   dnx_qual_info_p->size,
                                                                   dnx_qual_info_p->offset_on_key,
                                                                   &qualifier_ffc_info_p->
                                                                   ffc_info[ffc_index].ffc_initial,
                                                                   &internal_ipmf2_qual_info));

    SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                    (unit, &internal_ipmf2_qual_info, ffc_index, TRUE, FALSE,
                     &qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_instruction));

    SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id,
                     &(qualifier_ffc_info_p->ffc_info[ffc_index].ffc), internal_ipmf2_qual_info.ffc_type));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Qual %s internal_ipmf2_qual_info.offset 0x%X offset in key 0x%X %s\n",
                 dnx_field_dnx_qual_text(unit, dnx_qual_info_p->qual_type), internal_ipmf2_qual_info.offset,
                 qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.key_dest_offset, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_single_ffc_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    uint32 ffc_id,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_info_p)
{
    dbal_tables_e dbal_table_name;
    dbal_fields_e dbal_ffc_type;
    dbal_fields_e dbal_ffc_instruction;
    uint32 ffc_offset_uint32;
    uint32 ffc_field_index_uint32;

    uint32 entry_handle_id = 0;
    uint32 ffc_30_bit_instruction = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_instruction_info_p, _SHR_E_PARAM, "ffc_instruction_info_p");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &dbal_table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_instruction(unit, field_stage, &dbal_ffc_instruction));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_id);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "context_id %d field_stage %s ffc_id %d, program %d\n", context_id,
                 dnx_field_stage_text(unit, field_stage), ffc_id, context_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, dbal_ffc_instruction, INST_SINGLE,
                                                        &(ffc_instruction_info_p->full_instruction)));

    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                    (unit, dbal_ffc_instruction, &(ffc_instruction_info_p->full_instruction), &dbal_ffc_type,
                     &ffc_30_bit_instruction));

    if ((dbal_ffc_type != DBAL_FIELD_IPMF1_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_IPMF2_FFC_LITERALLY) &&
        (dbal_ffc_type != DBAL_FIELD_IPMF3_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_EPMF_FFC_LITERALLY) &&
        (dbal_ffc_type != DBAL_FIELD_FWD12_FFC_LITERALLY))
    {

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, dbal_ffc_type, DBAL_FIELD_FFC_INDEX,
                                                        &ffc_field_index_uint32, &ffc_30_bit_instruction));
        ffc_instruction_info_p->field_index = ffc_field_index_uint32;
    }
    else
    {

        ffc_instruction_info_p->field_index = 0;
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_OFFSET, &ffc_offset_uint32, &ffc_30_bit_instruction));
    ffc_instruction_info_p->offset = ffc_offset_uint32;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_SIZE, &(ffc_instruction_info_p->size),
                     &ffc_30_bit_instruction));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_KEY_OFFSET, &(ffc_instruction_info_p->key_offset),
                     &ffc_30_bit_instruction));

    ffc_instruction_info_p->ffc_type = dbal_ffc_type;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_size 0x%X, ffc_field_index %d, offset in key 0x%X ffc_offset 0x%X\n",
                 ffc_instruction_info_p->size, ffc_instruction_info_p->field_index, ffc_instruction_info_p->key_offset,
                 ffc_instruction_info_p->offset);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_instruction_hw_clear(
    int unit,
    dnx_field_context_t context_id,
    dbal_tables_e ffc_table_name,
    uint32 ffc_id)
{
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ffc_table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "FFC instruction delete ffc_id %d, context ID %d, %s%s\n",
                 ffc_id, context_id, EMPTY, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_hw_clear(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    uint32 key_id)
{
    uint32 entry_handle_id = 0;
    dbal_tables_e table_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "KBR delete key_id %d, context ID %d, %s%s\n", key_id, context_id, EMPTY, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_single_ffc_instruction_clear(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 ffc_id)
{
    dbal_tables_e ffc_table_name;
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Delete ffc_id %d field_stage %s context_id %d %s\n", ffc_id,
                 dnx_field_stage_text(unit, field_stage), context_id, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &ffc_table_name));

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_instruction_hw_clear(unit, context_id, ffc_table_name, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_initial_key_ffc_set(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    int ffc_index,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p)
{
    dnx_field_key_attached_qual_info_t initial_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    sal_memcpy(&initial_qual_info, qual_info_p, sizeof(initial_qual_info));

    initial_qual_info.offset_on_key = qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.key_dest_offset;

    SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                    (unit, &initial_qual_info, ffc_index, FALSE, TRUE,
                     &(qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_instruction)));

    SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                    (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                     &(qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc), initial_qual_info.ffc_type));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "ffc_id[%d] %d belongs to initial key %d key_dest_index 0x%X\n",
                 ffc_index, qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_id,
                 qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.key_id,
                 qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_instruction.key_offset);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p)
{
    int ffc_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    for (ffc_idx = 0;
         ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, qual_info_p, ffc_idx, FALSE, FALSE,
                         &qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, field_stage, context_id, &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc),
                         qual_info_p->ffc_type));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_initial_ffc_sharing_range_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID])
{
    dnx_field_context_hashing_info_t context_hashing_info;
    dnx_field_context_compare_info_t context_compare_info;
    dnx_field_context_state_table_info_t context_state_table_info;
    dnx_field_key_id_t key_id_hashing;
    dnx_field_key_id_t key_id_compare_pair_1_key_1;
    dnx_field_key_id_t key_id_compare_pair_1_key_2;
    dnx_field_key_id_t key_id_compare_pair_2_key_1;
    dnx_field_key_id_t key_id_compare_pair_2_key_2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_sharing_info_t_init(unit, context_ffc_sharing_info));

    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &context_hashing_info));
    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &context_compare_info));
    SHR_IF_ERR_EXIT(dnx_field_context_state_table_info_get(unit, context_id, &context_state_table_info));

    SHR_IF_ERR_EXIT(dnx_field_map_hash_key_id_get(unit, &key_id_hashing));
    SHR_IF_ERR_EXIT(dnx_field_map_compare_key_id_get(unit, DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1,
                                                     &key_id_compare_pair_1_key_1, &key_id_compare_pair_1_key_2));
    SHR_IF_ERR_EXIT(dnx_field_map_compare_key_id_get(unit, DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2,
                                                     &key_id_compare_pair_2_key_1, &key_id_compare_pair_2_key_2));

    if (context_hashing_info.mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        context_ffc_sharing_info[key_id_hashing.id[0]].is_key_id_valid = FALSE;
        context_ffc_sharing_info[key_id_hashing.id[1]].is_key_id_valid = FALSE;
    }
    if (context_compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        context_ffc_sharing_info[key_id_compare_pair_1_key_1.id[0]].offset_end =
            key_id_compare_pair_1_key_1.offset_on_first_key;
        context_ffc_sharing_info[key_id_compare_pair_1_key_2.id[0]].offset_end =
            key_id_compare_pair_1_key_2.offset_on_first_key;
    }
    if (context_compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        context_ffc_sharing_info[key_id_compare_pair_2_key_1.id[0]].offset_end =
            key_id_compare_pair_2_key_1.offset_on_first_key;
        context_ffc_sharing_info[key_id_compare_pair_2_key_2.id[0]].offset_end =
            key_id_compare_pair_2_key_2.offset_on_first_key;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_detach_is_sharing_initial(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_initial_ffc_info_t * intial_ffc_p,
    int *shared_p)
{
    int ffc_index;
    dbal_enum_value_field_field_key_e key_idx;
    dnx_field_context_t cascaded_from_context_id;
    uint32 ffc_id_list[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID];

    dnx_field_key_ffc_instruction_info_t ffc_instruction_info;
    dnx_field_metadata_qual_e ipmf2_base_qual;
    dnx_field_qual_t internal_ipmf2_qual_type;
    int mapped_offset;
    int nof_mappings;
    int offsets_arr[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    int first_ipmf2_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_keys_ipmf2_including_ipmf1 =
        first_ipmf2_key + dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    *shared_p = FALSE;

    if (intial_ffc_p->ffc.ffc_id == DNX_FIELD_FFC_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Initial FFC is invalid.\n");
    }

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Looking for initial FFCs for qualifier not on iPMF2, but on stage "
                     "%d (\"%s\").\n", field_stage, dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, field_stage, context_id, &cascaded_from_context_id));

    SHR_IF_ERR_EXIT(dnx_field_key_initial_ffc_sharing_range_get(unit, DNX_FIELD_STAGE_IPMF1,
                                                                cascaded_from_context_id, context_ffc_sharing_info));

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, intial_ffc_p->ffc.key_id, &ipmf2_base_qual));
    internal_ipmf2_qual_type = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &nof_mappings, offsets_arr));

    if (nof_mappings != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s for key %d split into %d parts, expected 1 part.\n",
                     dnx_field_dnx_qual_text(unit, internal_ipmf2_qual_type), intial_ffc_p->ffc.key_id, nof_mappings);
    }
    mapped_offset = offsets_arr[0];

    if ((context_ffc_sharing_info[intial_ffc_p->ffc.key_id].is_key_id_valid == FALSE) ||
        (intial_ffc_p->ffc.ffc_instruction.key_offset < context_ffc_sharing_info[intial_ffc_p->ffc.key_id].offset_start)
        || (intial_ffc_p->ffc.ffc_instruction.key_offset >=
            context_ffc_sharing_info[intial_ffc_p->ffc.key_id].offset_end))
    {
        SHR_EXIT();
    }

    for (key_idx = first_ipmf2_key; key_idx < nof_keys_ipmf2_including_ipmf1; key_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_idx, ffc_id_list));

        for (ffc_index = 0; ffc_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_index++)
        {
            if (ffc_id_list[ffc_index] != DNX_FIELD_FFC_ID_INVALID)
            {

                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get(unit, field_stage, context_id, ffc_id_list[ffc_index],
                                                                &ffc_instruction_info));

                if ((ffc_instruction_info.offset - mapped_offset == intial_ffc_p->ffc.ffc_instruction.key_offset) &&
                    (ffc_instruction_info.size == intial_ffc_p->ffc.ffc_instruction.size))
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE, "SHARED ffc_id %d in key %s size %d %s\n",
                                 ffc_id_list[ffc_index],
                                 dnx_field_dnx_qual_text(unit, internal_ipmf2_qual_type),
                                 ffc_instruction_info.size + 1, EMPTY);

                    *shared_p = TRUE;
                    SHR_EXIT();
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_attach_initial_is_shared(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_qual_t qual_type,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    int ffc_index,
    int *shared_p)
{
    int ffc_in_key_idx = 0;

    uint32 key_idx;
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID];
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_instruction_info_t ffc_instruction_info;
    dnx_field_key_ffc_instruction_info_t input_ffc_instruction;
    dbal_fields_e dbal_ffc_type;
    int first_initial_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_keys_including_initial = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Looking for initial FFCs for qualifier not on iPMF2, but on stage "
                     "%d (\"%s\").\n", field_stage, dnx_field_stage_text(unit, field_stage));
    }

    *shared_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_map_ffc_type_to_field(unit, DNX_FIELD_STAGE_IPMF1,
                                                    dnx_qual_info_p->ffc_type, &dbal_ffc_type));

    SHR_IF_ERR_EXIT(dnx_field_key_initial_ffc_sharing_range_get
                    (unit, field_stage, cascaded_from_context_id, context_ffc_sharing_info));

    SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                    (unit, dnx_qual_info_p, ffc_index, FALSE, TRUE, &input_ffc_instruction));

    for (key_idx = first_initial_key; key_idx < nof_keys_including_initial; key_idx++)
    {
        if (!context_ffc_sharing_info[key_idx].is_key_id_valid)
        {

            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                        (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, key_idx, ffc_id));

        for (ffc_in_key_idx = 0; ffc_in_key_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_in_key_idx++)
        {
            if (ffc_id[ffc_in_key_idx] != DNX_FIELD_FFC_ID_INVALID)
            {

                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, ffc_id[ffc_in_key_idx],
                                 &ffc_instruction_info));

                if ((ffc_instruction_info.ffc_type == dbal_ffc_type) &&
                    (ffc_instruction_info.field_index == input_ffc_instruction.field_index) &&
                    (ffc_instruction_info.offset == input_ffc_instruction.offset) &&
                    (ffc_instruction_info.size == input_ffc_instruction.size))
                {

                    if ((ffc_instruction_info.key_offset >= context_ffc_sharing_info[key_idx].offset_start) &&
                        (ffc_instruction_info.key_offset < context_ffc_sharing_info[key_idx].offset_end))
                    {

                        qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_id = ffc_id[ffc_in_key_idx];
                        qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.key_id = key_idx;
                        qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.key_dest_offset =
                            ffc_instruction_info.key_offset;

                        sal_memcpy(&qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_instruction,
                                   &input_ffc_instruction, sizeof(dnx_field_key_ffc_instruction_info_t));

                        LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id[%d] %d, from qualifier %s, in key_id %d is shared\n",
                                     ffc_in_key_idx,
                                     ffc_id[ffc_in_key_idx], dnx_field_dnx_qual_text(unit, qual_type), key_idx);
                        *shared_p = TRUE;
                        break;
                    }

                    continue;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_l4_ops_ffc_set(
    int unit,
    dnx_field_qual_t qual_type,
    uint32 ffc_id,
    dnx_field_qual_attach_info_t * qual_attach_info_p)
{
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info;
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    int nof_sub_quals;

    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);

    qualifier_ffc_info.qual_type = qual_type;
    qualifier_ffc_info.ffc_info[0].ffc.ffc_id = ffc_id;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, DNX_FIELD_STAGE_L4_OPS, 0, qual_type,
                                                0, qual_attach_info_p, &nof_sub_quals, dnx_qual_info));

    if (nof_sub_quals != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier %s (0x%x) is split into %d parts, not supported for range APIs.\n",
                     dnx_field_dnx_qual_text(unit, qual_type), qual_type, nof_sub_quals);
    }

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_set
                    (unit, DNX_FIELD_STAGE_L4_OPS, 0, &qualifier_ffc_info, &(dnx_qual_info[0])));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_ffc_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dbal_enum_value_field_field_key_e key_id,
    int use_specific_ffc_id,
    int forced_ffc_id)
{
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info;
    uint8 per_key_ranges;
    uint8 final_ranges;
    int is_shared = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    SHR_IF_ERR_EXIT(dnx_field_map_qual_key_to_ranges(unit, field_stage, key_id, &per_key_ranges));
    final_ranges = dnx_qual_info_p->ranges & per_key_ranges;

    dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);

    if ((DNX_QUAL_CLASS(dnx_qual_info_p->qual_type) == DNX_FIELD_QUAL_CLASS_SW)
        && (DNX_QUAL_ID(dnx_qual_info_p->qual_type) == DNX_FIELD_SW_QUAL_ALL_ZERO))
    {

        SHR_EXIT();
    }

    qualifier_ffc_info.qual_type = dnx_qual_info_p->qual_type;

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ffc_allocate_per_qualifier(unit, field_stage, context_id,
                                                                      dnx_qual_info_p->size, final_ranges,
                                                                      use_specific_ffc_id, forced_ffc_id,
                                                                      &qualifier_ffc_info),
                             "Failed to allocate FFCs for qualifier \"%s\" in stage \"%s\" for context %d.",
                             dnx_field_dnx_qual_text(unit, qualifier_ffc_info.qual_type),
                             dnx_field_stage_text(unit, field_stage), context_id);

    if ((dnx_qual_info_p->native_pbus == FALSE) && (field_stage == DNX_FIELD_STAGE_IPMF2))
    {
        dnx_field_context_t cascaded_from_context_id;
        int ffc_idx;

        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Qualifier %s size %d context_id %d cascaded_from_context_id %d\r\n",
                     dnx_field_dnx_qual_text(unit, qualifier_ffc_info.qual_type),
                     dnx_qual_info_p->size, context_id, cascaded_from_context_id);

        for (ffc_idx = 0; (ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL) &&
             (qualifier_ffc_info.ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
        {

            SHR_IF_ERR_EXIT(dnx_field_key_ffc_attach_initial_is_shared(unit, field_stage, cascaded_from_context_id,
                                                                       dnx_qual_info_p->qual_type, dnx_qual_info_p,
                                                                       &qualifier_ffc_info, ffc_idx, &is_shared));

            if (!is_shared)
            {
                SHR_IF_ERR_EXIT(dnx_field_key_ffc_allocate_per_initial_qualifier
                                (unit, cascaded_from_context_id, dnx_qual_info_p->qual_type, ffc_idx, dnx_qual_info_p,
                                 &qualifier_ffc_info));

                SHR_IF_ERR_EXIT(dnx_field_key_initial_key_ffc_set(unit, cascaded_from_context_id, ffc_idx,
                                                                  &qualifier_ffc_info, dnx_qual_info_p));

                SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add(unit, DNX_FIELD_STAGE_IPMF1,
                                                                    cascaded_from_context_id,
                                                                    qualifier_ffc_info.ffc_info[ffc_idx].
                                                                    ffc_initial.ffc.key_id,
                                                                    qualifier_ffc_info.ffc_info[ffc_idx].
                                                                    ffc_initial.ffc.ffc_id));
            }

            SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_ffc_set(unit, context_id, ffc_idx,
                                                        dnx_qual_info_p, &qualifier_ffc_info));
        }
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_set(unit, field_stage, context_id, &qualifier_ffc_info, dnx_qual_info_p));
    }

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_qualifier_ffc_hw_add(unit, field_stage, context_id, key_id, &qualifier_ffc_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_const_quals_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dbal_enum_value_field_field_key_e key_id)
{

    unsigned int nof_sub_qual = 0;
    unsigned int sub_qual_index;
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_SUB_QUAL];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_split_const
                    (unit, field_stage, dnx_qual_info_p, &dnx_qual_info_sub_qual[0], &nof_sub_qual));

    for (sub_qual_index = 0; sub_qual_index < nof_sub_qual; sub_qual_index++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                              &(dnx_qual_info_sub_qual[sub_qual_index]),
                                              key_id, FALSE, DNX_FIELD_FFC_ID_INVALID));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "subqual_index %d%s%s%s\n", sub_qual_index, EMPTY, EMPTY, EMPTY);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "offset %d,  %d size %d,  ffc input_type %d\n",
                     dnx_qual_info_sub_qual[sub_qual_index].offset,
                     dnx_qual_info_p->offset,
                     dnx_qual_info_sub_qual[sub_qual_index].size, dnx_qual_info_sub_qual[sub_qual_index].input_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * qual_types_p,
    dnx_field_key_template_t * key_template_p)
{
    int qual_idx;
    dnx_field_qual_map_in_key_t *key_qual_map_p;

    int offset_in_key = 0;
    int total_key_len = 0;
    uint32 qual_size = 0;
    int nof_mappings;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_types_p, _SHR_E_PARAM, "qual_types_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    key_template_p->key_size_in_bits = 0;

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
         && (qual_types_p[qual_idx] != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_types_p[qual_idx],
                                                    &nof_mappings, NULL, &qual_size));

        key_qual_map_p = &(key_template_p->key_qual_map[qual_idx]);

        key_qual_map_p->lsb = offset_in_key;
        key_qual_map_p->size = qual_size;
        key_qual_map_p->qual_type = qual_types_p[qual_idx];
        offset_in_key = key_qual_map_p->size + offset_in_key;

        if (total_key_len + key_qual_map_p->size < total_key_len)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key is too big, after qual number %d keyy size changed from %d to %d.\r\n",
                         qual_idx, total_key_len, total_key_len + key_qual_map_p->size);
        }
        total_key_len += key_qual_map_p->size;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Qual %s size %d, offset in key %d total_key_len %d\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), qual_size, key_qual_map_p->lsb,
                     total_key_len);
    }

    key_template_p->key_size_in_bits = total_key_len;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_lookup_type_get(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_group_type_e fg_type,
    int *is_found_p,
    dbal_enum_value_field_pmf_lookup_type_e * pmf_lookup_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_found_p, _SHR_E_PARAM, "is_found_p");
    SHR_NULL_CHECK(pmf_lookup_type_p, _SHR_E_PARAM, "pmf_lookup_type_p");

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            dbal_enum_value_field_field_key_e first_key_id;
            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_first_available_key_get(unit, field_stage, fg_type, &first_key_id));
            if (key_id == (first_key_id + 0))
            {
                *is_found_p = TRUE;
                *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0;
            }
            else if (key_id == (first_key_id + 1))
            {
                *is_found_p = TRUE;
                *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1;
            }
            else if (key_id == (first_key_id + 2))
            {
                *is_found_p = TRUE;
                *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2;
            }
            else if (key_id == (first_key_id + 3))
            {
                *is_found_p = TRUE;
                *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid TCAM key usage: key_id: %d can't be used as TCAM "
                             "on stage \"%s\".\r\n", key_id, dnx_field_stage_text(unit, field_stage));
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *is_found_p = TRUE;
            *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *is_found_p = TRUE;
            *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *is_found_p = TRUE;
            *pmf_lookup_type_p = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {

            *is_found_p = FALSE;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type! %d\r\n", fg_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_lookup_change_state_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_group_type_e fg_type,
    uint8 new_state)
{
    uint32 entry_handle_id;
    uint32 key_idx;
    int has_lookup_enabler;
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    int is_lookup_type_found;
    dbal_enum_value_field_pmf_lookup_type_e pmf_lookup_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_lookup_dbal_table_get(unit, field_stage, &has_lookup_enabler, &table_id));

    if (has_lookup_enabler == FALSE)
    {
        SHR_EXIT();
    }

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_lookup_type_get(unit, field_stage, key_id_p->id[key_idx], fg_type,
                                                      &is_lookup_type_found, &pmf_lookup_type));

        if (is_lookup_type_found == FALSE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PMF_LOOKUP_TYPE, pmf_lookup_type);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, new_state);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_quals_over_key_split(
    int unit,
    int *nof_mapping_p,
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL],
    int *second_nof_mapping_p,
    dnx_field_key_attached_qual_info_t second_dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL],
    uint8 *was_split_p)
{
    uint32 max_single_key_size;
    int sub_ndx_first;
    int nof_sub_quals_moved_to_second_key_completely = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info, _SHR_E_PARAM, "dnx_qual_info");
    SHR_NULL_CHECK(second_dnx_qual_info, _SHR_E_PARAM, "second_dnx_qual_info");

    (*was_split_p) = FALSE;

    max_single_key_size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    if ((*nof_mapping_p < 1) || (*nof_mapping_p > DNX_FIELD_KEY_MAX_SUB_QUAL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_mapping_p is %d, must be 1-%d.\n",
                     *nof_mapping_p, DNX_FIELD_KEY_MAX_SUB_QUAL);
    }

    if (dnx_qual_info[0].offset_on_key >= max_single_key_size)
    {
        int total_qual_size = 0;
        for (sub_ndx_first = 0; sub_ndx_first < *nof_mapping_p; sub_ndx_first++)
        {
            total_qual_size += dnx_qual_info[sub_ndx_first].size;
        }

        if ((dnx_qual_info[0].offset_on_key + dnx_qual_info[sub_ndx_first].size) > (2 * max_single_key_size))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s does not fit in second key. "
                         "offset in key(including lsb) %d qual_size %d nof_mapping %d.\n",
                         dnx_field_dnx_qual_text(unit, dnx_qual_info[0].qual_type), dnx_qual_info[0].offset_on_key,
                         total_qual_size, *nof_mapping_p);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------NO SPLIT: Qualifier %s fits the second key completely, "
                     "offset in key(including lsb) %d qual_size %d.%s \n",
                     dnx_field_dnx_qual_text(unit, dnx_qual_info[0].qual_type), dnx_qual_info[0].offset_on_key,
                     total_qual_size, EMPTY);

        SHR_FUNC_EXIT;
    }

    (*second_nof_mapping_p) = 0;
    for (sub_ndx_first = 0; sub_ndx_first < *nof_mapping_p; sub_ndx_first++)
    {
        if (dnx_qual_info[sub_ndx_first].offset_on_key >= max_single_key_size)
        {

            if ((dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size) >
                (2 * max_single_key_size))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "qual_size plus starting offset is %d, no support for size bigger than %d. \n",
                             dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size,
                             2 * max_single_key_size);
            }
            nof_sub_quals_moved_to_second_key_completely++;
            sal_memcpy(&(second_dnx_qual_info[*second_nof_mapping_p]), &(dnx_qual_info[sub_ndx_first]),
                       sizeof(second_dnx_qual_info[0]));
            (*second_nof_mapping_p)++;
            (*was_split_p) = TRUE;
        }
        else if (dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size > max_single_key_size)
        {

            if ((dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size) >
                (2 * max_single_key_size))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "qual_size plus starting offset is %d, no support for size bigger than %d. \n",
                             dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size,
                             2 * max_single_key_size);
            }

            if (*was_split_p)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Splitting sub qualifier %d/%d of %s, even though there was a split. \n",
                             sub_ndx_first, *nof_mapping_p,
                             dnx_field_dnx_qual_text(unit, dnx_qual_info[sub_ndx_first].qual_type));
            }

            sal_memcpy(&(second_dnx_qual_info[*second_nof_mapping_p]), &(dnx_qual_info[sub_ndx_first]),
                       sizeof(second_dnx_qual_info[0]));

            second_dnx_qual_info[*second_nof_mapping_p].size =
                dnx_qual_info[sub_ndx_first].offset_on_key + dnx_qual_info[sub_ndx_first].size - max_single_key_size;

            dnx_qual_info[sub_ndx_first].size = max_single_key_size - (dnx_qual_info[sub_ndx_first].offset_on_key);

            second_dnx_qual_info[*second_nof_mapping_p].offset_on_key += dnx_qual_info[sub_ndx_first].size;

            second_dnx_qual_info[*second_nof_mapping_p].offset += dnx_qual_info[sub_ndx_first].size;

            if (dnx_qual_info[sub_ndx_first].input_type == DNX_FIELD_INPUT_TYPE_CONST)
            {
                uint32 input_arg_uint32;
                input_arg_uint32 = dnx_qual_info[sub_ndx_first].index;

                if (input_arg_uint32 != dnx_qual_info[sub_ndx_first].index)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Input_arg %d mismatch with 32 bit unsigned integer %d.\r\n",
                                 dnx_qual_info[sub_ndx_first].index, input_arg_uint32);
                }
                if (dnx_qual_info[sub_ndx_first].size >= SAL_UINT32_NOF_BITS)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "First split size %d is bigger than %d bits.\r\n",
                                 dnx_qual_info[sub_ndx_first].size, SAL_UINT32_NOF_BITS - 1);
                }

                dnx_qual_info[sub_ndx_first].index =
                    input_arg_uint32 & (SAL_UPTO_BIT(dnx_qual_info[sub_ndx_first].size));
                second_dnx_qual_info[*second_nof_mapping_p].index =
                    input_arg_uint32 >> (dnx_qual_info[sub_ndx_first].size);
            }

            *was_split_p = TRUE;

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SPLIT first qual name %s, first  key lsb+offset %d first  qual_size %d first. %s\n",
                         dnx_field_dnx_qual_text(unit, dnx_qual_info[sub_ndx_first].qual_type),
                         dnx_qual_info[sub_ndx_first].offset_on_key, dnx_qual_info[sub_ndx_first].size, EMPTY);
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SPLIT second qual name %s, second key lsb+offset %d second qual_size %d.%s\n",
                         dnx_field_dnx_qual_text(unit, second_dnx_qual_info[*second_nof_mapping_p].qual_type),
                         second_dnx_qual_info[*second_nof_mapping_p].offset_on_key,
                         second_dnx_qual_info[*second_nof_mapping_p].size, EMPTY);

            (*second_nof_mapping_p)++;
        }
        else
        {

            if (*was_split_p)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not moving sub sub qualifier %d/%d of %s to second key, "
                             "even though there was a split. \n",
                             sub_ndx_first, *nof_mapping_p,
                             dnx_field_dnx_qual_text(unit, dnx_qual_info[sub_ndx_first].qual_type));
            }
        }
    }

    if ((nof_sub_quals_moved_to_second_key_completely != *second_nof_mapping_p) &&
        (nof_sub_quals_moved_to_second_key_completely != ((*second_nof_mapping_p) - 1)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "nof_sub_quals_moved_to_second_key_completely %d second_nof_mapping_p %d.\n",
                     nof_sub_quals_moved_to_second_key_completely, *second_nof_mapping_p);
    }

    for (sub_ndx_first = (*nof_mapping_p) - nof_sub_quals_moved_to_second_key_completely;
         sub_ndx_first < (*nof_mapping_p); sub_ndx_first++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_attached_qual_info_t_init(unit, &(dnx_qual_info[sub_ndx_first])));
    }

    (*nof_mapping_p) -= nof_sub_quals_moved_to_second_key_completely;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_quals_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    uint32 qual_index = 0;
    dnx_field_qual_t qual_type;
    int offset_from_first_key;
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    dnx_field_key_attached_qual_info_t split_dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    dnx_field_stage_e field_stage;
    uint8 was_split = FALSE;
    uint32 key_index = 0, next_key_index;
    int nof_sub_qual_mapping;
    int nof_sub_qual_mapping_split;
    int sub_qual_ndx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    field_stage = key_in_info_p->field_stage;

    for (qual_index = 0;
         (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_in_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_index++)
    {
        qual_type = key_in_info_p->key_template.key_qual_map[qual_index].qual_type;
        offset_from_first_key =
            key_in_info_p->key_template.key_qual_map[qual_index].lsb + key_id_p->offset_on_first_key;

        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, field_stage, context_id, qual_type, offset_from_first_key,
                                                    &key_in_info_p->qual_info[qual_index],
                                                    &nof_sub_qual_mapping, dnx_qual_info));

        was_split = FALSE;
        key_index = (dnx_qual_info[0].offset_on_key >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE) ? 1 : 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual_info[0].qual_type),
                     dnx_field_stage_text(unit, field_stage), dnx_qual_info[0].offset_on_key, key_index);

        if (key_index == 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SET: POTENTIAL SPLIT: for qual name %s, offset in key %d sub qual %d.%s\n",
                         dnx_field_dnx_qual_text(unit, dnx_qual_info[0].qual_type),
                         dnx_qual_info[0].offset_on_key, nof_sub_qual_mapping, EMPTY);

            SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit, &nof_sub_qual_mapping, dnx_qual_info,
                                                               &nof_sub_qual_mapping_split, split_dnx_qual_info,
                                                               &was_split));
        }
        if (dnx_qual_info[0].input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {

            SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                            (unit, field_stage, context_id, &dnx_qual_info[0], key_id_p->id[key_index]));
            if (was_split == TRUE)
            {
                next_key_index = key_index + 1;
                if (key_id_p->id[next_key_index] == DNX_FIELD_KEY_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no available key for split.\r\n");
                }
                SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                                (unit, field_stage, context_id, &split_dnx_qual_info[0], key_id_p->id[next_key_index]));
            }
            continue;
        }
        for (sub_qual_ndx = 0; sub_qual_ndx < nof_sub_qual_mapping; sub_qual_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id, &dnx_qual_info[sub_qual_ndx],
                                                  key_id_p->id[key_index], FALSE, DNX_FIELD_FFC_ID_INVALID));
        }
        if (was_split == TRUE)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Qaulifier %s split to %d and %d parts.%s\n",
                         dnx_field_dnx_qual_text(unit, dnx_qual_info[0].qual_type),
                         nof_sub_qual_mapping, nof_sub_qual_mapping_split, EMPTY);
            next_key_index = key_index + 1;
            if (key_id_p->id[next_key_index] == DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no available key for split.\r\n");
            }
            for (sub_qual_ndx = 0; sub_qual_ndx < nof_sub_qual_mapping_split; sub_qual_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id, &split_dnx_qual_info[sub_qual_ndx],
                                                      key_id_p->id[next_key_index], FALSE, DNX_FIELD_FFC_ID_INVALID));
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "SET Second SPLIT qual name %s, field_stage %s, offset in key %d sub qual %d.\n",
                             dnx_field_dnx_qual_text(unit, split_dnx_qual_info[sub_qual_ndx].qual_type),
                             dnx_field_stage_text(unit, field_stage),
                             split_dnx_qual_info[sub_qual_ndx].offset_on_key, sub_qual_ndx);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_lookup_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    dnx_algo_field_key_flags_e flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    field_stage = key_in_info_p->field_stage;
    fg_type = key_in_info_p->fg_type;

    if (key_in_info_p->compare_id != DNX_FIELD_GROUP_COMPARE_ID_NONE &&
        (key_in_info_p->field_stage != DNX_FIELD_STAGE_IPMF1 ||
         (key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_TCAM &&
          key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Compare used (%d), but compare can only be used in iPMF1 for TCAM/direct TCAM results. "
                     "stage %s, fg_type %s, context ID %d.\r\n", key_in_info_p->compare_id, dnx_field_stage_text(unit,
                                                                                                                 key_in_info_p->field_stage),
                     dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
    }

    switch (key_in_info_p->compare_id)
    {
        case DNX_FIELD_GROUP_COMPARE_ID_CMP_1:
            flags = flags | DNX_ALGO_FIELD_KEY_FLAG_WITH_ID;
            key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        case DNX_FIELD_GROUP_COMPARE_ID_CMP_2:
            flags = flags | DNX_ALGO_FIELD_KEY_FLAG_WITH_ID;
            key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                    (unit, flags, field_stage, fg_type, context_id, key_in_info_p->key_length_type, key_id_p));

    if (dnx_data_field.tests.epmf_debug_rely_on_tcam0_get(unit))
    {

        if (field_stage == DNX_FIELD_STAGE_EPMF
            && (key_id_p->id[0] == DBAL_ENUM_FVAL_FIELD_KEY_B || key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_B))
        {
            SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_clear(unit, context_id, field_stage, DBAL_ENUM_FVAL_FIELD_KEY_B));
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, field_stage, context_id, key_id_p, key_in_info_p->app_db_id));

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_tcam_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_key_attach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_de_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_de_bit_allocation_info_t * bit_range_allocation_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    dnx_algo_field_key_bit_range_align_t align_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(bit_range_allocation_info_p, _SHR_E_PARAM, "bit_range_allocation_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    field_stage = key_in_info_p->field_stage;
    fg_type = key_in_info_p->fg_type;
    sal_memset(&align_info, 0x0, sizeof(align_info));

    if (key_in_info_p->key_template.key_size_in_bits == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Direct extraction field group with 0 bits in key, please use field group type "
                     "const instead.\r\n");
    }
    if (bit_range_allocation_info_p->bit_range_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Direct extraction field groups only support key allocation by range.\r\n");
    }

    align_info.align_lsb = bit_range_allocation_info_p->bit_range_align_lsb;
    align_info.align_lsb_step = bit_range_allocation_info_p->bit_range_align_lsb_step;
    align_info.lsb_aligned_bit = bit_range_allocation_info_p->bit_range_lsb_aligned_offset;
    align_info.align_msb = bit_range_allocation_info_p->bit_range_align_msb;
    align_info.align_msb_step = bit_range_allocation_info_p->bit_range_align_msb_step;
    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate(unit, field_stage, fg_type, context_id,
                                                             key_in_info_p->key_template.key_size_in_bits, &align_info,
                                                             FALSE, key_id_p));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Allocated key %c and, on it, a range of %d bits at offset %d. "
                 "No alignment required. %s\r\n",
                 'A' + (int) (key_id_p->id[0]), key_id_p->nof_bits[0], key_id_p->offset_on_first_key, EMPTY);
    if (align_info.align_lsb)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Key aligned to lsb, granularity %d bit offset %d.%s%s\r\n",
                     align_info.align_lsb_step, align_info.lsb_aligned_bit, EMPTY, EMPTY);
    }
    if (align_info.align_msb)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Key aligned to msb, granularity %d.%s%s%s\r\n", align_info.align_lsb_step, EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_field_key_mdb_dt_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_algo_field_key_bit_range_align_t align_info;
    dbal_enum_value_field_field_key_e mdb_dt_key_id;

    SHR_FUNC_INIT_VARS(unit);

    switch (key_in_info_p->field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            mdb_dt_key_id = DBAL_ENUM_FVAL_FIELD_KEY_A + dnx_data_field.mdb_dt.ipmf1_key_select_get(unit);
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            mdb_dt_key_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc +
                dnx_data_field.mdb_dt.ipmf2_key_select_get(unit);
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            mdb_dt_key_id = DBAL_ENUM_FVAL_FIELD_KEY_A + dnx_data_field.mdb_dt.ipmf3_key_select_get(unit);
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        case DNX_FIELD_STAGE_ACE:
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stage \"%s\" not supported for MDB direct table.\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field stage %d.\n", key_in_info_p->field_stage);
        }
    }

    sal_memset(&align_info, 0x0, sizeof(align_info));
    align_info.align_lsb = FALSE;
    align_info.align_msb = FALSE;
    key_id_p->id[0] = mdb_dt_key_id;
    key_id_p->nof_bits[0] = dnx_data_field.mdb_dt.key_size_get(unit);
    key_id_p->offset_on_first_key = dnx_data_field.tcam.key_size_single_get(unit) - key_id_p->nof_bits[0];
    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate(unit, key_in_info_p->field_stage,
                                                             DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB, context_id,
                                                             key_id_p->nof_bits[0], &align_info, TRUE, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_exem_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_key_attach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_state_table_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    int use_acr;
    dnx_field_stage_e state_table_field_stage;
    dbal_enum_value_field_field_key_e single_key_id;
    int lsb_on_key;
    int state_table_key_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                    (unit, &use_acr, &state_table_field_stage, &single_key_id, &lsb_on_key, &state_table_key_size));

    {
        if (use_acr)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table lookup based on actions not supported. \n");
        }
        if (key_in_info_p->field_stage != state_table_field_stage)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table stage mismatch. stage is \"%s\" (%d), should be \"%s\" (%d).\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage), key_in_info_p->field_stage,
                         dnx_field_stage_text(unit, state_table_field_stage), state_table_field_stage);
        }
    }

    key_id_p->id[0] = single_key_id;
    key_id_p->offset_on_first_key = lsb_on_key;
    key_id_p->nof_bits[0] = state_table_key_size;

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_context_feature_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT)

static shr_error_e
dnx_field_key_epmf_at_least_one_ffc(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    if ((key_in_info_p->field_stage == DNX_FIELD_STAGE_EPMF)
        && (key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION))
    {
        int has_lookup_enabler;
        dbal_tables_e table_id;
        SHR_IF_ERR_EXIT(dnx_field_map_lookup_dbal_table_get(unit, key_in_info_p->field_stage,
                                                            &has_lookup_enabler, &table_id));
        if (has_lookup_enabler == FALSE)
        {
            uint32 ffc_id[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC] =
                { {0} };
            int key_ndx;
            int one_key_has_no_ffc = FALSE;
            int one_key_has_ffc = FALSE;
            int nof_keys_on_fg = 0;

            for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
            {
                if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
                {

                    if (nof_keys_on_fg != key_ndx)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Keys on FG are not continuous key_ndx %d.\n", key_ndx);
                    }

                    nof_keys_on_fg++;

                    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, key_in_info_p->field_stage, context_id,
                                                                       key_id_p->id[key_ndx], &(ffc_id[key_ndx][0])));
                    if (ffc_id[key_ndx][0] == DNX_FIELD_FFC_ID_INVALID)
                    {
                        one_key_has_no_ffc = TRUE;
                    }
                    else
                    {
                        one_key_has_ffc = TRUE;
                    }
                }
            }

            if (one_key_has_no_ffc)
            {
                if (nof_keys_on_fg <= 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached illogical option, one key has no FFC, but nof_keys %d.\n",
                                 nof_keys_on_fg);
                }
                else if (nof_keys_on_fg == 1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "An ePMF lookup must have at least one FFC "
                                 "(at least one qualifier that is not const zero). "
                                 "Field group type %s, context %d stage %s.\n",
                                 dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id,
                                 dnx_field_stage_text(unit, key_in_info_p->field_stage));
                }
                else if ((nof_keys_on_fg == 2) && (key_in_info_p->fg_type == DNX_FIELD_GROUP_TYPE_TCAM))
                {
                    if (one_key_has_ffc)
                    {

                        if ((key_id_p->id[0] != DNX_FIELD_KEY_ID_INVALID) && (ffc_id[0][0] != DNX_FIELD_FFC_ID_INVALID))
                        {
                            uint32 ffc_id_to_copy = DNX_FIELD_FFC_ID_INVALID;
                            int ffc_ndx;
                            dnx_field_key_ffc_instruction_info_t local_instruction;
                            int ffc_found = FALSE;

                            int prefix_size = dnx_data_field.tcam.max_prefix_size_get(unit);
                            int key_size = dnx_data_field.tcam.key_size_single_get(unit);
                            int fg_key_size = key_in_info_p->key_template.key_size_in_bits;
                            int second_key_size;
                            if (fg_key_size > key_size)
                            {
                                second_key_size = fg_key_size - key_size;
                            }
                            else
                            {
                                second_key_size = 0;
                            }

                            for (ffc_ndx = 0; (ffc_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) &&
                                 (ffc_id[0][ffc_ndx] != DNX_FIELD_FFC_ID_INVALID); ffc_ndx++)
                            {
                                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get(unit, key_in_info_p->field_stage,
                                                                                context_id, ffc_id[0][ffc_ndx],
                                                                                &local_instruction));
                                if (((local_instruction.key_offset + local_instruction.size) < (key_size - prefix_size))
                                    && (local_instruction.key_offset >= second_key_size))
                                {
                                    ffc_id_to_copy = ffc_id[0][ffc_ndx];
                                    ffc_found = TRUE;
                                    break;
                                }
                            }
                            if (ffc_found == FALSE)
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Both keys on a double key TCAM lookup on ePMF must have "
                                             "at least one FFC. No FFC on sencod key, and no FFC on first key that "
                                             "does not override the prefix or a qulifier on the second key."
                                             "context %d stage %s.\n",
                                             context_id, dnx_field_stage_text(unit, key_in_info_p->field_stage));
                            }

                            SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add
                                            (unit, key_in_info_p->field_stage, context_id, key_id_p->id[1],
                                             ffc_id_to_copy));
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "The first key on an ePMF double key TCAM lookup "
                                         "must have at least one FFC "
                                         "(at least one qualifier that is not const zero). "
                                         "context %d stage %s.\n",
                                         context_id, dnx_field_stage_text(unit, key_in_info_p->field_stage));
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "An ePMF lookup must have at least one FFC "
                                     "(at least one qualifier that is not const zero). "
                                     "Field group type %s, context %d stage %s.\n",
                                     dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id,
                                     dnx_field_stage_text(unit, key_in_info_p->field_stage));
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "An ePMF lookup must have at least one FFC. "
                                 "Field group type %s, context %d stage %s. Unexpected combination.\n",
                                 dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id,
                                 dnx_field_stage_text(unit, key_in_info_p->field_stage));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_epmf_at_least_one_ffc_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    int nof_ffcs,
    dnx_field_key_ffc_hw_info_t ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    if ((field_stage == DNX_FIELD_STAGE_EPMF) && (fg_type == DNX_FIELD_GROUP_TYPE_TCAM) &&
        (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID))
    {
        int has_lookup_enabler;
        dbal_tables_e table_id;
        SHR_IF_ERR_EXIT(dnx_field_map_lookup_dbal_table_get(unit, field_stage, &has_lookup_enabler, &table_id));
        if (has_lookup_enabler == FALSE)
        {
            int key_ndx;

            for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
            {
                if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
                {
                    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC] = { 0 };
                    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id,
                                                                       key_id_p->id[key_ndx], &(ffc_id[0])));

                    if ((ffc_id[0] != DNX_FIELD_FFC_ID_INVALID) && (ffc_id[1] != DNX_FIELD_FFC_ID_INVALID))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "An ePMF double key lookup has more than one undeleted FFC.\n");
                    }
                    if (ffc_id[0] != DNX_FIELD_FFC_ID_INVALID)
                    {
                        int ffc_ndx;
                        int ffc_deleted = FALSE;

                        for (ffc_ndx = 0; ffc_ndx < nof_ffcs; ffc_ndx++)
                        {
                            if (ffcs[ffc_ndx].ffc_id == ffc_id[0])
                            {
                                ffc_deleted = TRUE;
                                break;
                            }
                        }
                        if (ffc_deleted)
                        {

                            if (ffcs[ffc_ndx].key_id == key_id_p->id[key_ndx])
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For ePMF double key FFC %d on key %d was not deleted.\n",
                                             ffcs[ffc_ndx].ffc_id, ffcs[ffc_ndx].key_id);
                            }

                            SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove(unit, field_stage, context_id,
                                                                                   key_id_p->id[key_ndx], ffc_id[0]));
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "An ePMF double key lookup has an undeleted FFC.\n");
                        }

                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dnx_field_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_de_bit_allocation_info_t * bit_range_allocation_info_p,
    dnx_field_key_id_t *key_id_p)
{
    int key_ndx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, key_id_p));

    for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
    {
        if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id_p is not initialized, key %d isn't invalid. "
                         "stage %s, fg_type %s context ID %d.\n",
                         key_ndx, dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
        }
    }

    switch (key_in_info_p->fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_tcam_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_de_key_attach(unit, context_id, key_in_info_p, bit_range_allocation_info_p,
                                                        key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_exem_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_mdb_dt_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
        {
            SHR_ERR_EXIT(_SHR_E_DISABLED,
                         "Should not get here. Probably an internal error. Stage %s FG type: %s Attach to Context (%d)\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Field Group type = %d (%s) Not supported\n", key_in_info_p->fg_type,
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type));
            break;
        }
    }
#if defined(BCM_DNX2_SUPPORT)
    SHR_IF_ERR_EXIT(dnx_field_key_epmf_at_least_one_ffc(unit, context_id, key_in_info_p, key_id_p));
#endif

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_field_key_kbr_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id)
{
    uint32 app_db_id = (1 << dnx_data_field.tcam.app_db_id_size_get(unit)) - 1;
    uint32 ffc_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add(unit, field_stage, context_id, key_id, ffc_id));

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_set(unit, field_stage, context_id, key_id, app_db_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_to_qual_is_relative(
    int unit,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_info_p,
    uint32 *is_relative_p)
{

    uint32 qual_range_min;
    uint32 qual_range_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(ffc_instruction_info_p, _SHR_E_PARAM, "ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    qual_range_min = (dnx_qual_info_p->offset_on_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    qual_range_max =
        (dnx_qual_info_p->offset_on_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE + dnx_qual_info_p->size;

    *is_relative_p = FALSE;

    if ((ffc_instruction_info_p->key_offset >= qual_range_min) && (ffc_instruction_info_p->key_offset < qual_range_max))
    {
        *is_relative_p = TRUE;
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "qual_range_min %d qual_range_max %d ffc_instruction_info_p->key_offset %d dnx_qual_info_p->offset_on_key %d\n",
                 qual_range_min, qual_range_max, ffc_instruction_info_p->key_offset, dnx_qual_info_p->offset_on_key);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_to_initial_ffc_is_relative(
    int unit,
    dbal_enum_value_field_field_key_e inital_key_id,
    dnx_field_key_ffc_instruction_info_t * ipmf2_ffc_instruction_info_p,
    dnx_field_key_ffc_instruction_info_t * ipmf1_ffc_instruction_info_p,
    uint32 *is_relative_p)
{
    dnx_field_qual_t internal_ipmf2_qual_type;
    dnx_field_metadata_qual_e ipmf2_base_qual;
    int mapped_offset;
    int nof_mappings;
    int offsets_arr[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf2_ffc_instruction_info_p");
    SHR_NULL_CHECK(ipmf1_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf1_ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    *is_relative_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, inital_key_id, &ipmf2_base_qual));

    internal_ipmf2_qual_type = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &nof_mappings, offsets_arr));

    if (nof_mappings <= 0 || nof_mappings > DNX_FIELD_KEY_MAX_SUB_QUAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s split into %d parts, must be 1-%d.\n",
                     dnx_field_dnx_qual_text(unit, internal_ipmf2_qual_type), nof_mappings, DNX_FIELD_KEY_MAX_SUB_QUAL);
    }
    mapped_offset = offsets_arr[0];

    if (((ipmf2_ffc_instruction_info_p->offset) ==
         ipmf1_ffc_instruction_info_p->key_offset + mapped_offset)
        && (ipmf2_ffc_instruction_info_p->size == ipmf1_ffc_instruction_info_p->size))
    {
        *is_relative_p = TRUE;
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "ipmf2_ffc_instruction_info_p->offset 0x%X "
                     "ipmf1_ffc_instruction_info_p->key_offset 0x%X "
                     "mapped_offset 0x%X "
                     "size 0x%X\n",
                     ipmf2_ffc_instruction_info_p->offset,
                     ipmf1_ffc_instruction_info_p->key_offset, mapped_offset, ipmf2_ffc_instruction_info_p->size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_qual_ffc_get_internal(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 *output_ffc_index_p)
{
    uint32 ffc_idx;
    uint32 is_relative = 0;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_instruction_info_t local_instruction;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    qualifier_ffc_info_p->qual_type = dnx_qual_info_p->qual_type;

    for (ffc_idx = 0;
         (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_idx] != DNX_FIELD_FFC_ID_INVALID);
         ffc_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                        (unit, field_stage, context_id, ffc_id[ffc_idx], &local_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_to_qual_is_relative(unit, dnx_qual_info_p, &local_instruction, &is_relative));

        if (is_relative)
        {
            if (*output_ffc_index_p >= DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found more than %d FFCs per qualifier.\r\n",
                             DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL);
            }
            qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.ffc_id = ffc_id[ffc_idx];

            sal_memcpy(&(qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.ffc_instruction),
                       &local_instruction, sizeof(dnx_field_key_ffc_instruction_info_t));

            qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.key_id = key_id;

            (*output_ffc_index_p)++;

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "ffc_id[%d] %d belongs to qual %s output_ffc_index %d\n",
                         ffc_idx, ffc_id[ffc_idx], dnx_field_dnx_qual_text(unit, dnx_qual_info_p->qual_type),
                         (*output_ffc_index_p - 1));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_field_key_qual_ffc_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_key_attached_qual_info_t dummy_split_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    dnx_field_key_attached_qual_info_t dummy_dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    int nof_mapping;
    int nof_mapping_split;
    uint32 key_index = 0;
    uint8 qual_was_split = FALSE;
    uint32 ffc_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_attached_qual_info_t_init(unit, &dummy_dnx_qual_info[0]);
    dummy_dnx_qual_info[0].qual_type = key_qual_map_p->qual_type;
    dummy_dnx_qual_info[0].size = key_qual_map_p->size;
    dummy_dnx_qual_info[0].offset_on_key = key_qual_map_p->lsb + key_id_p->offset_on_first_key;
    nof_mapping = 1;

    key_index = ((dummy_dnx_qual_info[0].offset_on_key) >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE) ? 1 : 0;

    if (key_index >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only %d keys supported.\r\n", DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                 dnx_field_dnx_qual_text(unit, dummy_dnx_qual_info[0].qual_type), dnx_field_stage_text(unit,
                                                                                                       field_stage),
                 (dummy_dnx_qual_info[0].offset_on_key), key_index);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id, key_id_p->id[key_index],
                                                        &dummy_dnx_qual_info[0], qualifier_ffc_info_p, &ffc_idx));

    if (key_index == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------GET: POTENTIAL SPLIT: for qual name %s, offset in key (including LSB) %d qual_size %d.%s\n",
                     dnx_field_dnx_qual_text(unit, dummy_dnx_qual_info[0].qual_type),
                     dummy_dnx_qual_info[0].offset_on_key, dummy_dnx_qual_info[0].size, EMPTY);

        SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit, &nof_mapping, dummy_dnx_qual_info,
                                                           &nof_mapping_split, dummy_split_qual_info, &qual_was_split));

        if (nof_mapping != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After split nof_mapping %d (nof_mapping_split %d).\r\n",
                         nof_mapping, qual_was_split);
        }

        if (qual_was_split == TRUE)
        {

            if (key_index + 1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only %d keys supported, and qualifier is plit beyond that.\r\n",
                             DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);
            }

            if (nof_mapping_split != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "After split nof_mapping_split %d (nof_mapping %d).\r\n",
                             qual_was_split, nof_mapping);
            }

            SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id,
                                                                key_id_p->id[key_index + 1],
                                                                &dummy_split_qual_info[0],
                                                                qualifier_ffc_info_p, &ffc_idx));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------GET: The SECOND SPLIT: for qual name %s, offset in key (including LSB) %d qual_size %d ffc_idx %d \n",
                         dnx_field_dnx_qual_text(unit, dummy_split_qual_info[0].qual_type),
                         dummy_split_qual_info[0].offset_on_key, dummy_split_qual_info[0].size, ffc_idx);

        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "qual %s number of ffc's %d key_index %d size %d\n",
                 dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), ffc_idx, key_index, key_qual_map_p->size);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_ffc_hw_info_t * ipmf2_ffc_p,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p)
{
    uint32 ffc_idx;

    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 is_relative = 0;
    dnx_field_key_ffc_instruction_info_t initial_instr_temp;
    dbal_enum_value_field_field_key_e initial_key_id;
    dnx_field_context_t cascaded_from_context_id;
    int first_initial_key = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
    int nof_keys_including_initial = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_p, _SHR_E_PARAM, "ipmf2_ffc_p");
    SHR_NULL_CHECK(ipmf1_ffc_initial_p, _SHR_E_PARAM, "ipmf1_ffc_initial_p");

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    if (ipmf2_ffc_p->ffc_id != DNX_FIELD_FFC_ID_INVALID)
    {

        for (initial_key_id = first_initial_key; initial_key_id < nof_keys_including_initial; initial_key_id++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                            (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, initial_key_id, ffc_id));

            for (ffc_idx = 0;
                 (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_idx] != DNX_FIELD_FFC_ID_INVALID);
                 ffc_idx++)
            {

                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, ffc_id[ffc_idx],
                                 &initial_instr_temp));

                SHR_IF_ERR_EXIT(dnx_field_key_ffc_to_initial_ffc_is_relative
                                (unit, initial_key_id, &(ipmf2_ffc_p->ffc_instruction), &initial_instr_temp,
                                 &is_relative));
                if (is_relative)
                {

                    ipmf1_ffc_initial_p->ffc.ffc_id = ffc_id[ffc_idx];

                    sal_memcpy(&(ipmf1_ffc_initial_p->ffc.ffc_instruction),
                               &initial_instr_temp, sizeof(dnx_field_key_ffc_instruction_info_t));

                    ipmf1_ffc_initial_p->ffc.key_id = initial_key_id;
                    ipmf1_ffc_initial_p->key_dest_offset = initial_instr_temp.key_offset;

                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "ffc_id[%d] %d belongs to initial key %d key_dest_index 0x%X\n",
                                 ffc_idx, ffc_id[ffc_idx], ipmf1_ffc_initial_p->ffc.key_id,
                                 ipmf1_ffc_initial_p->key_dest_offset);

                    SHR_EXIT();
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_single_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e initial_key_id,
    uint32 ffc_id,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p)
{
    dnx_field_key_ffc_instruction_info_t initial_instr_temp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf1_ffc_initial_p, _SHR_E_PARAM, "ipmf1_ffc_initial_p");

    SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, ffc_id, &initial_instr_temp));

    ipmf1_ffc_initial_p->ffc.ffc_id = ffc_id;
    sal_memcpy(&(ipmf1_ffc_initial_p->ffc.ffc_instruction),
               &initial_instr_temp, sizeof(dnx_field_key_ffc_instruction_info_t));

    ipmf1_ffc_initial_p->ffc.key_id = initial_key_id;
    ipmf1_ffc_initial_p->key_dest_offset = initial_instr_temp.key_offset;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p,
    dnx_field_app_db_id_t * app_db_id_p)
{
    uint32 qual_idx;
    uint32 ffc_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");
    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    dnx_field_key_qualifier_ffc_info_init(group_ffc_info_p->key.qualifier_ffc_info,
                                          DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    if ((fg_type == DNX_FIELD_GROUP_TYPE_TCAM) || (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM) ||
        (fg_type == DNX_FIELD_GROUP_TYPE_EXEM))
    {
        SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_get(unit, field_stage, context_id, key_id_p->id[0], app_db_id_p));
    }

    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template_p->key_qual_map[qual_idx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type = key_template_p->key_qual_map[qual_idx].qual_type;

        SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get
                        (unit, field_stage, context_id, key_id_p,
                         &(key_template_p->key_qual_map[qual_idx]),
                         &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx])));

        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {

            if (DNX_QUAL_CLASS(key_template_p->key_qual_map[qual_idx].qual_type) != DNX_FIELD_QUAL_CLASS_META2)
            {

                for (ffc_idx = 0; (ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL) &&
                     (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
                      DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_ffc_get(unit, context_id,
                                                                       &(group_ffc_info_p->key.
                                                                         qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].
                                                                         ffc),
                                                                       &(group_ffc_info_p->key.
                                                                         qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].
                                                                         ffc_initial)));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_kbr_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int nof_ffcs,
    dnx_field_key_ffc_hw_info_t ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC],
    int nof_initial_ffcs,
    dnx_field_key_initial_ffc_info_t initial_ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 ffc_idx;
    int is_shared;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffcs, _SHR_E_PARAM, "ffcs");
    SHR_NULL_CHECK(initial_ffcs, _SHR_E_PARAM, "initial_ffcs");

    if ((nof_ffcs > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) || (nof_ffcs < 0) ||
        (nof_initial_ffcs > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) || (nof_initial_ffcs < 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "nof_initial_ffcs %d and nof_initial_ffcs %d must be 0-%d. Stage \"%s\" (%d). Context %d\r\n",
                     nof_ffcs, nof_initial_ffcs, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC,
                     dnx_field_stage_text(unit, field_stage), field_stage, context_id);
    }

    if ((nof_initial_ffcs != 0) && (field_stage != DNX_FIELD_STAGE_IPMF2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Stage \"%s\" (%d) is not ipmf2, but found %d initial FFCS. Context is %d.\r\n",
                     dnx_field_stage_text(unit, field_stage), field_stage, nof_initial_ffcs, context_id);
    }

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                     cascaded_from_context_id, EMPTY, EMPTY);
    }

    for (ffc_idx = 0; ffc_idx < nof_ffcs; ffc_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                        (unit, field_stage, context_id, ffcs[ffc_idx].key_id, ffcs[ffc_idx].ffc_id));
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate(unit, field_stage, context_id, ffcs[ffc_idx].ffc_id));

        SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                        (unit, field_stage, context_id, ffcs[ffc_idx].ffc_id));
    }

    {
        for (ffc_idx = 0; ffc_idx < nof_initial_ffcs; ffc_idx++)
        {
            if (initial_ffcs[ffc_idx].ffc.ffc_id == DNX_FIELD_FFC_ID_INVALID)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_key_ffc_detach_is_sharing_initial(unit, field_stage, context_id,
                                                                        &(initial_ffcs[ffc_idx]), &is_shared));

            LOG_DEBUG_EX(BSL_LOG_MODULE, "INITIAL FFC DELETE - ffc_id %d context %d is_shared %d stage %s\n",
                         initial_ffcs[ffc_idx].ffc.ffc_id, context_id, is_shared,
                         dnx_field_stage_text(unit, field_stage));

            if (!is_shared)
            {

                SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 initial_ffcs[ffc_idx].ffc.key_id, initial_ffcs[ffc_idx].ffc.ffc_id));

                SHR_IF_ERR_EXIT(dnx_field_key_ipmf1_initial_key_occupation_bmp_ffc_deallocate
                                (unit, cascaded_from_context_id, &(initial_ffcs[ffc_idx])));

                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 initial_ffcs[ffc_idx].ffc.ffc_id));

                SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 initial_ffcs[ffc_idx].ffc.ffc_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_group_ffc_info_to_array(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p,
    int *nof_ffcs_p,
    dnx_field_key_ffc_hw_info_t ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC],
    int *nof_initial_ffcs_p,
    dnx_field_key_initial_ffc_info_t initial_ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 qual_idx;
    int ffc_in_group_info_idx;
    int ffc_in_array_idx;
    int initial_ffc_in_group_info_idx;
    int initial_ffc_in_array_idx;
    dnx_field_key_ffc_hw_info_t *ffc_p;
    dnx_field_key_initial_ffc_info_t *ffc_initial_p;
    int ffc_idx_2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");
    SHR_NULL_CHECK(nof_ffcs_p, _SHR_E_PARAM, "nof_ffcs_p");
    SHR_NULL_CHECK(ffcs, _SHR_E_PARAM, "ffcs");
    SHR_NULL_CHECK(nof_initial_ffcs_p, _SHR_E_PARAM, "nof_initial_ffcs_p");
    SHR_NULL_CHECK(initial_ffcs, _SHR_E_PARAM, "initial_ffcs");

    ffc_in_array_idx = 0;
    initial_ffc_in_array_idx = 0;
    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {

        for (ffc_in_group_info_idx = 0; ffc_in_group_info_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL;
             ffc_in_group_info_idx++)
        {
            ffc_p = &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_in_group_info_idx].ffc);
            if (ffc_p->ffc_id != DNX_FIELD_FFC_ID_INVALID)
            {

                if (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Qualifier number %d is invalid, but has an FFC (ID %d, number on qualifier %d). "
                                 "Stage is %s.\r\n",
                                 qual_idx, ffc_p->ffc_id, ffc_in_group_info_idx, dnx_field_stage_text(unit,
                                                                                                      field_stage));
                }

                for (ffc_idx_2 = 0; ffc_idx_2 < ffc_in_array_idx; ffc_idx_2++)
                {
                    if (ffc_p->ffc_id == ffcs[ffc_idx_2].ffc_id)
                    {
                        break;
                    }
                }
                if (ffc_idx_2 < ffc_in_array_idx)
                {

                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "FFC %d is not an initial FFC, "
                                 "but is used more than once. Stage is %s.\r\n",
                                 ffc_p->ffc_id, dnx_field_stage_text(unit, field_stage));
                }

                if (ffc_in_array_idx >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Qualifier number %d FFC number %d exceeds maximum number of FFCs %d. Stage is %s.\r\n",
                                 qual_idx, ffc_in_group_info_idx, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC,
                                 dnx_field_stage_text(unit, field_stage));
                }
                sal_memcpy(&(ffcs[ffc_in_array_idx]), ffc_p, sizeof(ffcs[0]));
                ffc_in_array_idx++;
            }
        }

        for (initial_ffc_in_group_info_idx = 0; initial_ffc_in_group_info_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL;
             initial_ffc_in_group_info_idx++)
        {
            ffc_initial_p =
                &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx].
                  ffc_info[initial_ffc_in_group_info_idx].ffc_initial);
            if (ffc_initial_p->ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID)
            {

                if (field_stage != DNX_FIELD_STAGE_IPMF2)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Stage \"%s\" (%d) is not ipmf2, but found initial FFC %d (number %d) "
                                 "in qualifier 0x%x (\"%s\").\r\n",
                                 dnx_field_stage_text(unit, field_stage), field_stage,
                                 ffc_initial_p->ffc.ffc_id, initial_ffc_in_group_info_idx,
                                 group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type,
                                 dnx_field_dnx_qual_text(unit,
                                                         group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type));
                }

                if (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Qualifier number %d is invalid, but has an initial FFC (ID %d, number on qualifier %d). "
                                 "Stage is %s.\r\n",
                                 qual_idx, ffc_initial_p->ffc.ffc_id, initial_ffc_in_group_info_idx,
                                 dnx_field_stage_text(unit, field_stage));
                }

                if (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[initial_ffc_in_group_info_idx].
                    ffc.ffc_id == DNX_FIELD_FFC_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Qualifier number %d FFC number %d is invalid, but has initial FFC %d. "
                                 "Stage is %s.\r\n",
                                 qual_idx, initial_ffc_in_group_info_idx, ffc_initial_p->ffc.ffc_id,
                                 dnx_field_stage_text(unit, field_stage));
                }

                for (ffc_idx_2 = 0; ffc_idx_2 < initial_ffc_in_array_idx; ffc_idx_2++)
                {
                    if (ffc_initial_p->ffc.ffc_id == initial_ffcs[ffc_idx_2].ffc.ffc_id)
                    {
                        break;
                    }
                }
                if (ffc_idx_2 < initial_ffc_in_array_idx)
                {
                    continue;
                }

                if (initial_ffc_in_array_idx >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Qualifier number %d initial FFC number %d exceeds maximum number of FFCs %d. "
                                 "Stage is %s.\r\n",
                                 qual_idx, initial_ffc_in_group_info_idx, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC,
                                 dnx_field_stage_text(unit, field_stage));
                }
                sal_memcpy(&(initial_ffcs[initial_ffc_in_array_idx]), ffc_initial_p, sizeof(initial_ffcs[0]));
                initial_ffc_in_array_idx++;
            }
        }
    }

    (*nof_ffcs_p) = ffc_in_array_idx;
    (*nof_initial_ffcs_p) = initial_ffc_in_array_idx;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_ffc_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_group_ffc_info_t *group_ffc_info_p = NULL;
    dnx_field_app_db_id_t app_db_id;
    int nof_ffcs;
    dnx_field_key_ffc_hw_info_t ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    int nof_initial_ffcs;
    dnx_field_key_initial_ffc_info_t initial_ffcs[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_ALLOC(group_ffc_info_p, sizeof(*group_ffc_info_p), "group_ffc_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_key_info_get
                    (unit, field_stage, context_id, fg_type, key_id_p, key_template_p, group_ffc_info_p, &app_db_id));

    SHR_IF_ERR_EXIT(dnx_field_key_group_ffc_info_to_array(unit, field_stage, group_ffc_info_p,
                                                          &nof_ffcs, ffcs, &nof_initial_ffcs, initial_ffcs));

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_kbr_deallocate(unit, field_stage, context_id,
                                                     nof_ffcs, ffcs, nof_initial_ffcs, initial_ffcs));

#if defined(BCM_DNX2_SUPPORT)

    SHR_IF_ERR_EXIT(dnx_field_key_epmf_at_least_one_ffc_detach(unit, field_stage, context_id, fg_type, key_id_p,
                                                               nof_ffcs, ffcs));
#endif

exit:
    SHR_FREE(group_ffc_info_p);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_de_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, field_stage, context_id, fg_type, key_template_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free(unit, field_stage, fg_type, context_id, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_tcam_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, field_stage, context_id, fg_type, key_template_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free(unit, fg_type, field_stage, key_length_type, context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set(unit, field_stage, context_id, key_id_p, fg_type, FALSE));

    if (dnx_data_field.tests.epmf_debug_rely_on_tcam0_get(unit))
    {

        if (field_stage == DNX_FIELD_STAGE_EPMF
            && (key_id_p->id[0] == DBAL_ENUM_FVAL_FIELD_KEY_B || key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_B))
        {
            SHR_IF_ERR_EXIT(dnx_field_key_kbr_init(unit, DNX_FIELD_STAGE_EPMF, context_id, DBAL_ENUM_FVAL_FIELD_KEY_B));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_state_table_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, field_stage, context_id, fg_type, key_template_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set(unit, field_stage, context_id, key_id_p, fg_type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_exem_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, field_stage, context_id, fg_type, key_template_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free(unit, fg_type, field_stage, key_length_type, context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set(unit, field_stage, context_id, key_id_p, fg_type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_mdb_dt_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, field_stage, context_id, fg_type, key_template_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free(unit, field_stage, fg_type, context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set(unit, field_stage, context_id, key_id_p, fg_type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_context_feature_key_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, stage, context_id, DNX_FIELD_GROUP_TYPE_INVALID,
                                                 key_template_p, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            SHR_IF_ERR_EXIT(dnx_field_key_tcam_key_detach(unit, field_stage, context_id, fg_type,
                                                          key_template_p, key_length_type, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            SHR_IF_ERR_EXIT(dnx_field_key_de_key_detach(unit, field_stage, context_id, fg_type,
                                                        key_template_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_EXEM:
            SHR_IF_ERR_EXIT(dnx_field_key_exem_key_detach(unit, field_stage, context_id, fg_type,
                                                          key_template_p, key_length_type, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_key_detach(unit, field_stage, context_id, fg_type,
                                                                 key_template_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
            SHR_IF_ERR_EXIT(dnx_field_key_mdb_dt_key_detach(unit, field_stage, context_id, fg_type,
                                                            key_template_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
            SHR_ERR_EXIT(_SHR_E_DISABLED,
                         "Should not get here. Probably an internal error. Stage %s FG type: type %s Detach from Context (%d)\r\n",
                         dnx_field_stage_text(unit, field_stage), dnx_field_group_type_e_get_name(fg_type), context_id);
            break;
        case DNX_FIELD_GROUP_TYPE_CONST:

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage %s FG type: type %s Detach from Context (%d) not supported. Unknown field group type.\r\n",
                         dnx_field_stage_text(unit, field_stage), dnx_field_group_type_e_get_name(fg_type), context_id);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_field_key_alloc_sw_state_init(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_id_t_init(
    int unit,
    dnx_field_key_id_t *key_p)
{
    int key_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_p, _SHR_E_PARAM, "key_p");

    sal_memset(key_p, 0x0, sizeof(*key_p));

    for (key_index = 0; key_index < (sizeof(key_p->id) / sizeof(key_p->id[0])); key_index++)
    {
        key_p->id[key_index] = DNX_FIELD_KEY_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_single_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p)
{
    uint32 key_idx;
    int compare_key_starting_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");
    SHR_NULL_CHECK(tcam_key_id_p, _SHR_E_PARAM, "tcam_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Compare context %d mode Single key_id %d Tcam key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], tcam_key_id_p->id[0], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         TRUE, &(initial_key_id_p->id[key_idx]), &compare_key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_single_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p)
{
    uint32 key_idx;
    int compare_key_starting_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");
    SHR_NULL_CHECK(tcam_key_id_p, _SHR_E_PARAM, "tcam_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Compare context %d mode Single key_id %d Tcam key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], tcam_key_id_p->id[0], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         initial_key_id_p->id[key_idx], compare_key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_double_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p)
{
    uint32 key_idx;
    int compare_key_starting_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_1_p, _SHR_E_PARAM, "initial_key_id_1_p");
    SHR_NULL_CHECK(initial_key_id_2_p, _SHR_E_PARAM, "initial_key_id_2_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Compare context %d mode Double key_id %d key_id %d %s\n",
                 context_id, initial_key_id_1_p->id[0], initial_key_id_2_p->id[0], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_1_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_1_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         TRUE, &(initial_key_id_1_p->id[key_idx]), &compare_key_starting_offset));
    }

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_2_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_2_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         TRUE, &(initial_key_id_2_p->id[key_idx]), &compare_key_starting_offset));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_double_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p)
{
    uint32 key_idx;
    int compare_key_starting_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_1_p, _SHR_E_PARAM, "initial_key_id_1_p");
    SHR_NULL_CHECK(initial_key_id_2_p, _SHR_E_PARAM, "initial_key_id_2_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Compare context %d mode Double key_id %d key_id %d %s\n",
                 context_id, initial_key_id_1_p->id[0], initial_key_id_2_p->id[0], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_1_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_1_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         initial_key_id_1_p->id[key_idx], compare_key_starting_offset));

    }

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_2_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        if (key_idx == 0)
        {
            compare_key_starting_offset = initial_key_id_2_p->offset_on_first_key;
        }
        else
        {
            compare_key_starting_offset = 0;
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                        (unit, context_id,
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         initial_key_id_2_p->id[key_idx], compare_key_starting_offset));

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_hash_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    int key_starting_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Hash context %d key_id %d key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], initial_key_id_p->id[1], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate
                        (unit, context_id, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE,
                         TRUE, &(initial_key_id_p->id[key_idx]), &key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_hash_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    uint32 key_starting_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Hash context %d key_id %d key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], initial_key_id_p->id[1], EMPTY);

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                        (unit, context_id, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE,
                         initial_key_id_p->id[key_idx], key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_state_table_resources_reserve(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id)
{
    int use_acr;
    dnx_field_stage_e state_table_field_stage;
    dbal_enum_value_field_field_key_e single_key_id;
    int lsb_on_key;
    int state_table_key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                    (unit, &use_acr, &state_table_field_stage, &single_key_id, &lsb_on_key, &state_table_key_size));

    if (use_acr)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table lookup based on actions not supported.\n");
    }

    if (context_field_stage != state_table_field_stage &&
        (context_field_stage != DNX_FIELD_STAGE_IPMF1 || state_table_field_stage != DNX_FIELD_STAGE_IPMF2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error trying to reserve bits for state table. "
                     "Stage for state table \"%s\" (%d). Stage for context %d \"%s\" (%d).\n",
                     dnx_field_stage_text(unit, state_table_field_stage), state_table_field_stage, context_id,
                     dnx_field_stage_text(unit, context_field_stage), context_field_stage);
    }

    if (state_table_field_stage == DNX_FIELD_STAGE_IPMF1)
    {

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Reserving resources for state table, context %d, key_id %d, bits %d-%d, iPMF1.\r\n",
                     context_id, single_key_id, lsb_on_key + state_table_key_size - 1, lsb_on_key);

        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate
                        (unit, context_id, state_table_key_size, TRUE, &single_key_id, &lsb_on_key));
    }
    else if (state_table_field_stage == DNX_FIELD_STAGE_IPMF2)
    {

        dnx_algo_field_key_bit_range_align_t align_info;
        dnx_field_key_id_t key_id;

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Allocating bits for state table, context %d, key_id %d, bits %d-%d, iPMF2.\r\n",
                     context_id, single_key_id, lsb_on_key + state_table_key_size - 1, lsb_on_key);

        sal_memset(&align_info, 0x0, sizeof(align_info));
        align_info.align_lsb = FALSE;
        align_info.align_msb = FALSE;
        SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, &key_id));
        key_id.id[0] = single_key_id;
        key_id.offset_on_first_key = lsb_on_key;
        key_id.nof_bits[0] = state_table_key_size;
        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate(unit, state_table_field_stage,
                                                                 DNX_FIELD_GROUP_TYPE_STATE_TABLE, context_id,
                                                                 state_table_key_size, &align_info, TRUE, &key_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported stage for state table \"%s\" (%d). \n",
                     dnx_field_stage_text(unit, state_table_field_stage), state_table_field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_state_table_resources_free(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id)
{
    int use_acr;
    dnx_field_stage_e state_table_field_stage;
    dbal_enum_value_field_field_key_e single_key_id;
    int lsb_on_key;
    int state_table_key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                    (unit, &use_acr, &state_table_field_stage, &single_key_id, &lsb_on_key, &state_table_key_size));

    if (use_acr)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table lookup based on actions not supported.\n");
    }

    if (context_field_stage != state_table_field_stage &&
        (context_field_stage != DNX_FIELD_STAGE_IPMF1 || state_table_field_stage != DNX_FIELD_STAGE_IPMF2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error trying to free reserved bits for state table. "
                     "Stage for state table \"%s\" (%d). Stage for context %d \"%s\" (%d).\n",
                     dnx_field_stage_text(unit, state_table_field_stage), state_table_field_stage, context_id,
                     dnx_field_stage_text(unit, context_field_stage), context_field_stage);
    }

    if (state_table_field_stage == DNX_FIELD_STAGE_IPMF1)
    {

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Freeing resources for state table, context %d, key_id %d, bits %d-%d, iPMF1.\r\n",
                     context_id, single_key_id, lsb_on_key + state_table_key_size - 1, lsb_on_key);

        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                        (unit, context_id, state_table_key_size, single_key_id, lsb_on_key));
    }
    else if (state_table_field_stage == DNX_FIELD_STAGE_IPMF2)
    {

        dnx_field_key_id_t key_id;

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Deallocating bits for state table, context %d, key_id %d, bits %d-%d, iPMF2.\r\n",
                     context_id, single_key_id, lsb_on_key + state_table_key_size - 1, lsb_on_key);

        SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, &key_id));
        key_id.id[0] = single_key_id;
        key_id.offset_on_first_key = lsb_on_key;
        key_id.nof_bits[0] = state_table_key_size;
        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                        (unit, state_table_field_stage, DNX_FIELD_GROUP_TYPE_STATE_TABLE, context_id, &key_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported stage for state table \"%s\" (%d). \n",
                     dnx_field_stage_text(unit, state_table_field_stage), state_table_field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_init_verify(
    int unit)
{
    uint8 is_enum_illegal;
    uint32 key_enum_value;
    int nof_keys_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                    (unit, DBAL_FIELD_FIELD_KEY, DNX_FIELD_KEY_ID_INVALID, &is_enum_illegal));
    if (is_enum_illegal == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "DNX_FIELD_KEY_ID_INVALID (%d) is a valid value for DBAL ENUM FIELD_KEY.\n",
                     DNX_FIELD_KEY_ID_INVALID);
    }

    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_FIELD_KEY, INVALID_ENUM, &key_enum_value));
    while (key_enum_value != INVALID_ENUM)
    {
        if (key_enum_value >= DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "DBAL ENUM FIELD_KEY has value %d, exceeds DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID (%d).\n",
                         key_enum_value, DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID);
        }
        if (key_enum_value != nof_keys_found)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "DBAL ENUM FIELD_KEY is not continuous. key_enum_value=%d nof_keys_found=%d.\n",
                         key_enum_value, nof_keys_found);
        }
        nof_keys_found++;
        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_FIELD_KEY, key_enum_value, &key_enum_value));
    }

    if (nof_keys_found !=
        (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc +
         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_keys_alloc))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "DBAL ENUM FIELD_KEY has %d keys. iPMF1 has %d and iPMF2 hs %d.\n",
                     key_enum_value, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_keys_alloc);
    }

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                    (unit, DBAL_FIELD_FIELD_KEY, DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID, &is_enum_illegal));
    if (is_enum_illegal == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID (%d) is a valid value for DBAL ENUM FIELD_KEY.\n",
                     DNX_FIELD_KEY_MAX_NOF_FIELD_KEY_ID);
    }

exit:
    SHR_FUNC_EXIT;
}
