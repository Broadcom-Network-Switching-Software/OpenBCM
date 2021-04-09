
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
#include <bcm_int/dnx/kbp/kbp_mngr.h>
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
    dnx_field_qual_attach_info_t * qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info");

    sal_memset(qual_info_p, 0x0, sizeof(*qual_info_p));

    qual_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;

    qual_info_p->input_arg = BCM_FIELD_INVALID;
    qual_info_p->offset = BCM_FIELD_INVALID;

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
    key_in_info_p->fg_type = DNX_FIELD_GROUP_INVALID;
    key_in_info_p->field_stage = DNX_FIELD_STAGE_INVALID;
    key_in_info_p->key_length = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
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
    attached_qual_info_p->ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
    attached_qual_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;
    attached_qual_info_p->index = BCM_FIELD_INVALID;
    attached_qual_info_p->offset = BCM_FIELD_INVALID;
    attached_qual_info_p->ranges = 0;
    attached_qual_info_p->size = 0;
    attached_qual_info_p->native_pbus = FALSE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_sharing_info_t_init(
    int unit,
    dnx_field_key_ffc_sharing_info_t info[DBAL_NOF_ENUM_FIELD_KEY_VALUES])
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(info[0]) * DBAL_NOF_ENUM_FIELD_KEY_VALUES);

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
    dnx_field_ipmf2_qual_e * qual_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_id, _SHR_E_PARAM, "qual_id");

    switch (initial_key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_G_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_H_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_I_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_J_INITIAL;
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
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_3;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1;
            }
            else
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2;
            }
            else
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3:
        {
            if (second_part_payload)
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_3;
            }
            else
            {
                qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2;
            }
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_LEXEM:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_EXEM_ACTION;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_DIRECT_ACTION;
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
    dnx_field_context_t context_id,
    dnx_field_qual_t dnx_qual,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_qual_class_e qual_class = DNX_QUAL_CLASS(dnx_qual);
    int qual_size = dnx_qual_info_p->size;
    int ffc_idx;
    int ffc_size = 0;
    int bit_offset;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    for (ffc_idx = 0; qual_size > 0; ffc_idx++, qual_size -= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        if (ffc_idx >= DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier requires more than %d FFCs.\n", DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL);
        }
        ffc_size = ((qual_size > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) ?
                    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC : qual_size);

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_allocate(unit,
                                                                                              cascaded_from_context_id,
                                                                                              ffc_size, FALSE,
                                                                                              &
                                                                                              (qualifier_ffc_info_p->ffc_info
                                                                                               [ffc_idx].ffc_initial.
                                                                                               ffc.key_id),
                                                                                              &bit_offset),
                                 "Failed to allocate place in initial key for FFC %d for qualifier \"%s\""
                                 "for context %d.", ffc_idx, dnx_field_dnx_qual_text(unit,
                                                                                     qualifier_ffc_info_p->qual_type),
                                 context_id);
        qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset = bit_offset;

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_allocate_initial_ffc(unit, cascaded_from_context_id,
                                                                    dnx_qual_info_p->ranges,
                                                                    &(qualifier_ffc_info_p->
                                                                      ffc_info[ffc_idx].ffc_initial.ffc.ffc_id)),
                                 "Failed to allocate FFC %d in initial key for qualifier \"%s\" for context %d.",
                                 ffc_idx, dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type), context_id);

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "allocated initial ffc&key for qual_type %s key_id %d qual_size %d field_stage ipmf1 dest_offset %d\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual),
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.key_id,
                     qual_size, qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction.key_offset);
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE, "initial_key qual_type 0x%x class %d size %d cascaded_from_context_id %d\n",
                 dnx_qual, qual_class, dnx_qual_info_p->size, cascaded_from_context_id);
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

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Read ffc bitmap context_id %d, key_id %d ffc_bmp 0x%X%X\n",
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
        SHR_ERR_EXIT(_SHR_E_FULL, "FFC array is full ");
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
dnx_field_key_kbr_qualifier_initial_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_index = 0;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    cascaded_from_context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    for (ffc_index = 0; (ffc_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add(unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                                            qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.
                                                            ffc.key_id,
                                                            qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.
                                                            ffc.ffc_id));
    }
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
    uint32 ffc_index = 0;
    dnx_field_qual_class_e qual_class = DNX_FIELD_QUAL_CLASS_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    for (ffc_index = 0;
         (ffc_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add
                        (unit, field_stage, context_id, key_id, qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id));
    }

    qual_class = DNX_QUAL_CLASS(qualifier_ffc_info_p->qual_type);
    if ((qual_class != DNX_FIELD_QUAL_CLASS_META2) && (field_stage == DNX_FIELD_STAGE_IPMF2))
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_qualifier_initial_ffc_hw_add(unit, DNX_FIELD_STAGE_IPMF1, context_id,
                                                                       qualifier_ffc_info_p));
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
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_qual_map_in_key_t key_qual_map_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL],
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL],
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
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(key_qual_map_sub_qual, _SHR_E_PARAM, "key_qual_map_sub_qual");
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

    all_ones_qual = DNX_FIELD_ACTION_INVALID;
    all_ones_offset = 0;
    all_ones_size = 0;
    if (in_const != 0)
    {
        rv = dnx_field_map_field_id_dnx_qual(unit, field_stage, DBAL_FIELD_ALL_ONES, &all_ones_qual);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage \"%s\" does not support non-zero constant qualifiers. Input argument for const "
                         "qualifier 0x%x (\"%s\") is 0x%x.\r\n",
                         dnx_field_stage_text(unit, field_stage),
                         key_qual_map_p->qual_type, dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type),
                         dnx_qual_info_p->index);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, all_ones_qual, &all_ones_offset));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, all_ones_qual, &all_ones_size));
    }

    all_zeros_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_SW, field_stage, DNX_FIELD_SW_QUAL_ALL_ZERO);

    sub_position = 0;
    while (sub_position < dnx_qual_info_p->size)
    {

        if (*nof_sub_qual_p >= DNX_FIELD_KEY_MAX_CONST_SUB_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Number of sub qualifiers reached maximum (%d) and still  %d bits left in "
                         "const (out of %d).\r\n",
                         DNX_FIELD_KEY_MAX_CONST_SUB_QUAL, dnx_qual_info_p->size - sub_position, dnx_qual_info_p->size);
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
        sal_memcpy(&(key_qual_map_sub_qual[*nof_sub_qual_p]), key_qual_map_p, sizeof(key_qual_map_sub_qual[0]));

        dnx_qual_info_sub_qual[*nof_sub_qual_p].size = length_sub;
        key_qual_map_sub_qual[*nof_sub_qual_p].size = length_sub;
        key_qual_map_sub_qual[*nof_sub_qual_p].lsb += sub_position;
        if (is_first_digit_set)
        {
            key_qual_map_sub_qual[*nof_sub_qual_p].qual_type = all_ones_qual;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].ffc_type = DNX_FIELD_FFC_LITERALLY;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].offset = all_ones_offset;
        }
        else
        {
            key_qual_map_sub_qual[*nof_sub_qual_p].qual_type = all_zeros_qual;
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
        && (key_qual_map_sub_qual[*nof_sub_qual_p].qual_type != all_zeros_qual))
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
    dnx_field_qual_attach_info_t * qual_info_p)
{
    dnx_field_action_length_type_e action_length_type;
    dnx_field_stage_e cascaded_field_stage;
    int min_offset = -1;
    int max_offset = -1;
    int intermediate_point = -1;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    if (qual_info_p->input_type != DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be reached for input type cascaded. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, field_stage));
    }

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from other PMF stages only supported in iPMF2. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, qual_info_p->input_arg, &cascaded_field_stage));
    if (cascaded_field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from PMF stages can only cascade from stage iPMF1. "
                     "(input type %d, input argument (field group) %d, "
                     "qualifier %s, cascaded field group stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, cascaded_field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, qual_info_p->input_arg, &action_length_type));

    switch (action_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
        case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
        case DNX_FIELD_ACTION_LENGTH_TYPE_ACE:
        case DNX_FIELD_ACTION_LENGTH_TYPE_MDB_DT:
        {
            unsigned int block_size;
            min_offset = 0;
            SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(unit, action_length_type, &block_size));
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
                         qual_info_p->input_type, qual_info_p->input_arg,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
        default:
        {
            dnx_field_group_type_e fg_type;
            SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, qual_info_p->input_arg, &fg_type));
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown action_length_type %d. "
                         "(input type %d, input argument (field group) %d, fg_type %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         action_length_type,
                         qual_info_p->input_type, qual_info_p->input_arg, fg_type,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &qual_size));
    if (qual_info_p->offset < min_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) cannot be smaller then (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, min_offset, qual_info_p->input_type, qual_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }
    if (qual_info_p->offset + qual_size > max_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) plus qualifier size (%d) (together %d) cannot exceed (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, qual_size, qual_info_p->offset + qual_size, max_offset,
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

    if (qual_info_p->offset < intermediate_point && intermediate_point < qual_info_p->offset + qual_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascading qualifier for double key result can't span both "
                     "key payloads. Qualifier start: %d,  end: %d. intermediate point: %d (input type %d,"
                     "input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, qual_info_p->offset + qual_size, intermediate_point,
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p)
{
    dnx_field_qual_class_e dnx_qual_class;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    dnx_qual_class = DNX_QUAL_CLASS(qual_type);

    switch (qual_info_p->input_type)
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
                SHR_ERR_EXIT(_SHR_E_PARAM, "Input type layer (header) can only support header or "
                             "user defined qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
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
                             "Input type KBP can only support user defined qualifiers. "
                             "Got qual class %s (qualifier %s).\r\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_dnx_qual_text(unit, qual_type));
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
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Input type %d for qualifier %s is not supported.\r\n",
                         qual_info_p->input_type, dnx_field_dnx_qual_text(unit, qual_type));
        }
            break;
    }

    if ((qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE)
        || (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE))
    {
        if (qual_info_p->input_arg < 0)
            SHR_ERR_EXIT(_SHR_E_PARAM, "Absolute input types cannot take negative layer index "
                         "(input type %d, input argument %d, qualifier %s).\r\n",
                         qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

    if (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_cascaded_verify(unit, field_stage, qual_type, qual_info_p));
    }

    if (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_EXTERNAL)
    {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_qual_info_get_kbp_verify(unit, field_stage, qual_type, qual_info_p));
#else
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\r\n");
#endif
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
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &action_buffer_offset));

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
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p)
{
    int index;
    int mapped_offset;
    int layer_record_offset;
    int base_offset;
    int added_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_key_qual_info_get_verify
                          (unit, field_stage, qual_type, qual_info_p, dnx_qual_info_p));

    SHR_IF_ERR_EXIT(dnx_field_key_attached_qual_info_t_init(unit, dnx_qual_info_p));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, qual_type, &mapped_offset));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &(dnx_qual_info_p->size)));

    if (DNX_QUAL_CLASS(qual_type) == DNX_FIELD_QUAL_CLASS_USER)
    {
        base_offset = mapped_offset + qual_info_p->offset;
        added_offset = 0;
    }
    else
    {

        base_offset = mapped_offset;
        added_offset = qual_info_p->offset;
    }

    dnx_qual_info_p->input_type = qual_info_p->input_type;

    switch (qual_info_p->input_type)
    {
        case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_RELATIVE_HEADER;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset =
                DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset, dnx_qual_info_p->size);
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_ABSOLUTE_HEADER;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset =
                DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset, dnx_qual_info_p->size);
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;

            dnx_qual_info_p->index = 0;
            dnx_qual_info_p->offset = base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA2:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;

            dnx_qual_info_p->index = 0;
            dnx_qual_info_p->offset = base_offset;
            dnx_qual_info_p->native_pbus = TRUE;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LAYER_RECORD;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset = base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = 0;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_record_offset(unit, field_stage, index, &layer_record_offset));
            dnx_qual_info_p->offset = layer_record_offset + base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_EXTERNAL:
        {

            int kbp_payload_offset;
            dnx_field_qual_t dnx_qual_kbp_result;

            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            dnx_qual_info_p->index = 0;

            dnx_qual_kbp_result =
                DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_0);
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, dnx_qual_kbp_result, &kbp_payload_offset));

            dnx_qual_info_p->offset = kbp_payload_offset + base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CASCADED:
        {
            int absolute_qual_offset = 0;

            SHR_IF_ERR_EXIT(dnx_field_key_prepare_cascaded_qual_info(unit, field_stage, context_id,
                                                                     qual_info_p->input_arg, qual_info_p->offset,
                                                                     &absolute_qual_offset,
                                                                     &(dnx_qual_info_p->native_pbus)));

            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;

            dnx_qual_info_p->index = qual_info_p->input_arg;

            dnx_qual_info_p->offset = absolute_qual_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CONST:
        {

            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
            dnx_qual_info_p->index = qual_info_p->input_arg;
            dnx_qual_info_p->offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported input type: %d.\r\n", qual_info_p->input_type);
        }
    }

    if (dnx_qual_info_p->offset < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated offset (%d) is negative.\r\n", dnx_qual_info_p->offset);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s offset %d original offset %d.\n",
                 dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                 dnx_qual_info_p->offset, qual_info_p->offset);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s qual_info_p->input_type %d size %d.\n",
                 dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                 qual_info_p->input_type, dnx_qual_info_p->size);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_to_ranges
                    (unit, field_stage, qual_info_p->input_type, dnx_qual_info_p->offset, &(dnx_qual_info_p->ranges)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_qual_info_uses_up_to_one_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    uint8 *is_up_to_one_ffc_p)
{
    dnx_field_key_attached_qual_info_t dnx_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, field_stage, DNX_FIELD_CONTEXT_ID_INVALID, qual_type, qual_info_p, &dnx_qual_info));

    (*is_up_to_one_ffc_p) = TRUE;

    if (dnx_qual_info.input_type == DNX_FIELD_INPUT_TYPE_CONST)
    {
        dnx_field_qual_map_in_key_t key_qual_map;
        dnx_field_qual_map_in_key_t key_qual_map_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];
        dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];
        unsigned int nof_sub_qual;

        sal_memset(&key_qual_map, 0x0, sizeof(key_qual_map));

        key_qual_map.lsb = 0;
        key_qual_map.qual_type = qual_type;
        key_qual_map.size = dnx_qual_info.size;
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_split_const(unit, field_stage, &key_qual_map, &dnx_qual_info,
                                                                key_qual_map_sub_qual, dnx_qual_info_sub_qual,
                                                                &nof_sub_qual));
        if (nof_sub_qual > 1)
        {
            (*is_up_to_one_ffc_p) = FALSE;
        }
    }

    if (dnx_qual_info.size > dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit))
    {
        (*is_up_to_one_ffc_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_calc_ffc_instruction_values(
    int unit,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    uint32 ffc_index,
    uint32 is_read_from_initial,
    uint32 is_initial,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    uint32 template_offset_within_key,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_p)
{
    int qual_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
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

        ffc_instruction_p->key_offset = (key_qual_map_p->lsb) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    }
    else
    {

        ffc_instruction_p->key_offset =
            (key_qual_map_p->lsb +
             ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC +
             template_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

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
    dnx_field_key_attached_qual_info_t * internal_ipmf2_qual_info_p)
{
    dnx_field_qual_t internal_ipmf2_dnx_qual;
    dnx_field_ipmf2_qual_e ipmf2_base_qual = DNX_FIELD_QUAL_ID_INVALID;
    dnx_field_qual_attach_info_t qual_attach_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &qual_attach_info));
    qual_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, key_id, &ipmf2_base_qual));

    internal_ipmf2_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, internal_ipmf2_dnx_qual, &qual_attach_info,
                     internal_ipmf2_qual_info_p));

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
    dnx_field_key_initial_ffc_info_t * initial_ffc_info_p,
    dnx_field_key_attached_qual_info_t * internal_ipmf2_qual_info_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_ffc_info_p, _SHR_E_PARAM, "initial_ffc_info_p");
    SHR_NULL_CHECK(internal_ipmf2_qual_info_p, _SHR_E_PARAM, "internal_ipmf2_qual_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_convert_initial_key_id_into_meta2_qual
                    (unit, context_id, initial_ffc_info_p->ffc.key_id, internal_ipmf2_qual_info_p));

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
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 template_offset_within_key)
{
    dnx_field_key_attached_qual_info_t internal_ipmf2_qual_info;

    int ffc_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    dnx_field_key_attached_qual_info_t_init(unit, &internal_ipmf2_qual_info);

    for (ffc_idx = 0;
         (ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_prepare_internal_qual_info(unit,
                                                                       context_id,
                                                                       key_qual_map_p->size,
                                                                       &qualifier_ffc_info_p->
                                                                       ffc_info[ffc_idx].ffc_initial,
                                                                       &internal_ipmf2_qual_info));

        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, &internal_ipmf2_qual_info, ffc_idx, TRUE, FALSE, key_qual_map_p,
                         template_offset_within_key, &qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, DNX_FIELD_STAGE_IPMF2, context_id,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc), internal_ipmf2_qual_info.ffc_type));

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Qual %s internal_ipmf2_qual_info.offset 0x%X offset in key 0x%X %s\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), internal_ipmf2_qual_info.offset,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset, EMPTY);
    }
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
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p)
{
    int ffc_idx;
    dnx_field_qual_map_in_key_t initial_key_qual_map;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    initial_key_qual_map.qual_type = key_qual_map_p->qual_type;
    initial_key_qual_map.size = key_qual_map_p->size;

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Qualifier %s size %d context_id %d cascaded_from_context_id %d\r\n",
                 dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type),
                 key_qual_map_p->size, context_id, cascaded_from_context_id);

    for (ffc_idx = 0;
         ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {

        initial_key_qual_map.lsb = qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset;

        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, qual_info_p, ffc_idx, FALSE, TRUE, &initial_key_qual_map, 0,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction)));

        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc), qual_info_p->ffc_type));

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "ffc_id[%d] %d belongs to initial key %d key_dest_index 0x%X\n",
                     ffc_idx, qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_id,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.key_id,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction.key_offset);

    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p,
    uint32 template_offset_within_key)
{
    int ffc_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    for (ffc_idx = 0;
         ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, qual_info_p, ffc_idx, FALSE, FALSE, key_qual_map_p,
                         template_offset_within_key, &qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_instruction));

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
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DBAL_NOF_ENUM_FIELD_KEY_VALUES])
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
dnx_field_key_ffc_is_sharing_initial(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p,
    dnx_field_key_initial_ffc_info_t * intial_ffc_p,
    int *shared_p)
{
    int ffc_index;
    int qual_index;
    dbal_enum_value_field_field_key_e key_idx;
    dnx_field_context_t cascaded_from_context_id;
    uint32 ffc_id_list[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DBAL_NOF_ENUM_FIELD_KEY_VALUES];

    dnx_field_key_ffc_instruction_info_t ffc_instruction_info;
    dnx_field_key_attached_qual_info_t internal_ipmf2_qual_info;
    dnx_field_ipmf2_qual_e ipmf2_base_qual;
    dnx_field_qual_t internal_ipmf2_qual_type;
    int mapped_offset;
    SHR_FUNC_INIT_VARS(unit);

    *shared_p = FALSE;

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(_SHR_E_DISABLED);
    }

    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_prepare_internal_qual_info(unit,
                                                                   context_id,
                                                                   0, intial_ffc_p, &internal_ipmf2_qual_info));

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, field_stage, context_id, &cascaded_from_context_id));

    SHR_IF_ERR_EXIT(dnx_field_key_initial_ffc_sharing_range_get(unit, DNX_FIELD_STAGE_IPMF1,
                                                                cascaded_from_context_id, context_ffc_sharing_info));

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, intial_ffc_p->ffc.key_id, &ipmf2_base_qual));
    internal_ipmf2_qual_type = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &mapped_offset));

    if ((context_ffc_sharing_info[intial_ffc_p->ffc.key_id].is_key_id_valid == FALSE) ||
        (intial_ffc_p->ffc.ffc_instruction.key_offset < context_ffc_sharing_info[intial_ffc_p->ffc.key_id].offset_start)
        || (intial_ffc_p->ffc.ffc_instruction.key_offset >=
            context_ffc_sharing_info[intial_ffc_p->ffc.key_id].offset_end))
    {
        SHR_EXIT();
    }

    for (qual_index = 0; (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (group_ffc_info_p->key.qualifier_ffc_info[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_index++)
    {
        for (ffc_index = 0; ffc_index < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL; ffc_index++)
        {
            uint32 ffc_id;
            uint32 internal_ffc_idx;
            uint32 internal_qual_idx;
            ffc_id = group_ffc_info_p->key.qualifier_ffc_info[qual_index].ffc_info[ffc_index].ffc_initial.ffc.ffc_id;
            if (ffc_id != DNX_FIELD_FFC_ID_INVALID)
            {

                for (internal_qual_idx = qual_index + 1; (internal_qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                     && (group_ffc_info_p->key.qualifier_ffc_info[internal_qual_idx].qual_type !=
                         DNX_FIELD_QUAL_TYPE_INVALID); internal_qual_idx++)
                {
                    for (internal_ffc_idx = 0; internal_ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL; internal_ffc_idx++)
                    {
                        if ((ffc_id ==
                             group_ffc_info_p->key.qualifier_ffc_info[internal_qual_idx].
                             ffc_info[internal_ffc_idx].ffc_initial.ffc.ffc_id) && (ffc_id == intial_ffc_p->ffc.ffc_id))
                        {
                            group_ffc_info_p->key.qualifier_ffc_info[internal_qual_idx].
                                ffc_info[internal_ffc_idx].ffc_initial.ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
                        }
                    }
                }

                for (internal_ffc_idx = 0; internal_ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL; internal_ffc_idx++)
                {
                    if ((ffc_index != internal_ffc_idx) && (ffc_id ==
                                                            group_ffc_info_p->key.qualifier_ffc_info[qual_index].
                                                            ffc_info[internal_ffc_idx].ffc_initial.ffc.ffc_id)
                        && (ffc_id == intial_ffc_p->ffc.ffc_id))
                    {
                        group_ffc_info_p->key.qualifier_ffc_info[qual_index].ffc_info[internal_ffc_idx].ffc_initial.
                            ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
                    }
                }
            }
        }
    }

    for (key_idx = DBAL_ENUM_FVAL_FIELD_KEY_F; key_idx < DBAL_NOF_ENUM_FIELD_KEY_VALUES; key_idx++)
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
dnx_field_key_ffc_is_shared(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_t qual_type,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    int *shared_p)
{
    int ffc_idx = 0;
    int ffc_in_key_idx = 0;

    dnx_field_qual_map_in_key_t initial_key_qual_map;
    dnx_field_context_t cascaded_from_context_id;
    uint32 key_idx;
    dnx_field_key_ffc_sharing_info_t context_ffc_sharing_info[DBAL_NOF_ENUM_FIELD_KEY_VALUES];
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_instruction_info_t ffc_instruction_info;
    dnx_field_key_ffc_instruction_info_t input_ffc_instruction;
    dbal_fields_e dbal_ffc_type;

    int found_match = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(_SHR_E_DISABLED);
    }

    *shared_p = FALSE;

    initial_key_qual_map.qual_type = qual_type;
    initial_key_qual_map.size = dnx_qual_info_p->size;

    SHR_IF_ERR_EXIT(dnx_field_map_ffc_type_to_field(unit, DNX_FIELD_STAGE_IPMF1,
                                                    dnx_qual_info_p->ffc_type, &dbal_ffc_type));

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, field_stage, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Qualifier %s size %d context_id %d cascaded_from_context_id %d\r\n",
                 dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type),
                 dnx_qual_info_p->size, context_id, cascaded_from_context_id);

    SHR_IF_ERR_EXIT(dnx_field_key_initial_ffc_sharing_range_get
                    (unit, field_stage, cascaded_from_context_id, context_ffc_sharing_info));

    for (ffc_idx = 0; ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
         (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID) &&
         (found_match == TRUE); ffc_idx++)
    {

        found_match = FALSE;

        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, dnx_qual_info_p, ffc_idx, FALSE, TRUE, &initial_key_qual_map, 0,
                         &input_ffc_instruction));

        for (key_idx = DBAL_ENUM_FVAL_FIELD_KEY_F; key_idx < DBAL_NOF_ENUM_FIELD_KEY_VALUES; key_idx++)
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

                            qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_id = ffc_id[ffc_in_key_idx];
                            qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.key_id = key_idx;
                            qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset =
                                ffc_instruction_info.key_offset;

                            sal_memcpy(&qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction,
                                       &input_ffc_instruction, sizeof(dnx_field_key_ffc_instruction_info_t));

                            LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id[%d] %d, from qualifier %s, in key_id %d is shared\n",
                                         ffc_in_key_idx,
                                         ffc_id[ffc_in_key_idx], dnx_field_dnx_qual_text(unit, qual_type), key_idx);
                            found_match = TRUE;
                        }

                        continue;
                    }
                }
            }
        }
    }
    if ((found_match == TRUE) && (ffc_idx != 0))
    {
        *shared_p = TRUE;
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
    dnx_field_qual_map_in_key_t key_qual_map;
    dnx_field_key_attached_qual_info_t dnx_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);
    dnx_field_key_attached_qual_info_t_init(unit, &dnx_qual_info);

    qualifier_ffc_info.qual_type = qual_type;
    qualifier_ffc_info.ffc_info[0].ffc.ffc_id = ffc_id;
    key_qual_map.lsb = 0;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, DNX_FIELD_STAGE_L4_OPS, 0, qual_type,
                                                qual_attach_info_p, &dnx_qual_info));

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_set
                    (unit, DNX_FIELD_STAGE_L4_OPS, 0, &key_qual_map, &qualifier_ffc_info, &dnx_qual_info, 0));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_ffc_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dbal_enum_value_field_field_key_e key_id,
    uint32 template_offset_within_key,
    int use_specific_ffc_id,
    int forced_ffc_id)
{
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info;
    uint8 per_key_ranges;
    uint8 final_ranges;
    int is_shared = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    SHR_IF_ERR_EXIT(dnx_field_map_qual_key_to_ranges(unit, field_stage, key_id, &per_key_ranges));
    final_ranges = dnx_qual_info_p->ranges & per_key_ranges;

    dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);

    if ((DNX_QUAL_CLASS(key_qual_map_p->qual_type) == DNX_FIELD_QUAL_CLASS_SW)
        && (DNX_QUAL_ID(key_qual_map_p->qual_type) == DNX_FIELD_SW_QUAL_ALL_ZERO))
    {

        SHR_EXIT();
    }

    qualifier_ffc_info.qual_type = key_qual_map_p->qual_type;

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ffc_allocate_per_qualifier(unit, field_stage, context_id,
                                                                      dnx_qual_info_p->size, final_ranges,
                                                                      use_specific_ffc_id, forced_ffc_id,
                                                                      &qualifier_ffc_info),
                             "Failed to allocate FFCs for qualifier \"%s\" in stage \"%s\" for context %d.",
                             dnx_field_dnx_qual_text(unit, qualifier_ffc_info.qual_type),
                             dnx_field_stage_text(unit, field_stage), context_id);

    if ((dnx_qual_info_p->native_pbus == FALSE) && (field_stage == DNX_FIELD_STAGE_IPMF2))
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_is_shared(unit, field_stage, context_id, key_qual_map_p->qual_type,
                                                    dnx_qual_info_p, &qualifier_ffc_info, &is_shared));

        if (!is_shared)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_ffc_allocate_per_initial_qualifier
                            (unit, context_id, key_qual_map_p->qual_type, dnx_qual_info_p, &qualifier_ffc_info));

            SHR_IF_ERR_EXIT(dnx_field_key_initial_key_ffc_set
                            (unit, context_id, key_qual_map_p, &qualifier_ffc_info, dnx_qual_info_p));
        }

        SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_ffc_set
                        (unit, context_id, key_qual_map_p, &qualifier_ffc_info, template_offset_within_key));
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_set
                        (unit, field_stage, context_id, key_qual_map_p, &qualifier_ffc_info, dnx_qual_info_p,
                         template_offset_within_key));
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
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dbal_enum_value_field_field_key_e key_id,
    uint32 template_offset_within_key)
{

    unsigned int nof_sub_qual = 0;
    unsigned int sub_qual_index;
    dnx_field_qual_map_in_key_t key_qual_map_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_split_const
                    (unit, field_stage, key_qual_map_p,
                     dnx_qual_info_p, &key_qual_map_sub_qual[0], &dnx_qual_info_sub_qual[0], &nof_sub_qual));

    for (sub_qual_index = 0; sub_qual_index < nof_sub_qual; sub_qual_index++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                              &key_qual_map_sub_qual[sub_qual_index],
                                              &(dnx_qual_info_sub_qual[sub_qual_index]),
                                              key_id, template_offset_within_key, FALSE, DNX_FIELD_FFC_ID_INVALID));

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
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_types_p, _SHR_E_PARAM, "qual_types_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    key_template_p->key_size_in_bits = 0;

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
         && (qual_types_p[qual_idx] != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_types_p[qual_idx], &qual_size));

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
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_group_type_e fg_type,
    dbal_enum_value_field_pmf_lookup_type_e * pmf_lookup_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pmf_lookup_type, _SHR_E_PARAM, "pmf_lookup_type");

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            switch (key_id)
            {
                case DBAL_ENUM_FVAL_FIELD_KEY_A:
                case DBAL_ENUM_FVAL_FIELD_KEY_F:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_B:
                case DBAL_ENUM_FVAL_FIELD_KEY_G:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_C:
                case DBAL_ENUM_FVAL_FIELD_KEY_H:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_D:
                case DBAL_ENUM_FVAL_FIELD_KEY_I:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid TCAM key usage: key_id: %d can't be used as TCAM\r\n", key_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {

            *pmf_lookup_type = DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES;
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
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    dbal_enum_value_field_pmf_lookup_type_e pmf_lookup_type = DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_lookup_dbal_table_get(unit, field_stage, &table_id));

    if (table_id == DBAL_NOF_TABLES)
    {
        SHR_EXIT();
    }

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_lookup_type_get(unit, key_id_p->id[key_idx], fg_type, &pmf_lookup_type));

        if (pmf_lookup_type == DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES)
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
    uint32 offset_within_key,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_qual_map_in_key_t * second_qual_map_p,
    dnx_field_key_attached_qual_info_t * second_dnx_qual_info_p,
    uint8 *was_split_p)
{
    uint32 max_single_key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(second_qual_map_p, _SHR_E_PARAM, "second_qual_map_p");
    SHR_NULL_CHECK(second_dnx_qual_info_p, _SHR_E_PARAM, "second_dnx_qual_info_p");

    max_single_key_size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    if (key_qual_map_p->lsb + offset_within_key >= max_single_key_size)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------NO SPLIT: Qualifier %s fits the second key completely, offset in key %d qual_size %d lsb+offset %d \n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                     key_qual_map_p->size, (key_qual_map_p->lsb + offset_within_key));

        SHR_FUNC_EXIT;
    }

    if (key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size > max_single_key_size)
    {
        if (key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size > 2 * max_single_key_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "qual_size plus starting offset is %d, no support for size bigger than %d. \n",
                         key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size, 2 * max_single_key_size);
        }

        sal_memcpy(second_qual_map_p, key_qual_map_p, sizeof(*second_qual_map_p));
        sal_memcpy(second_dnx_qual_info_p, dnx_qual_info_p, sizeof(*second_dnx_qual_info_p));

        second_qual_map_p->size = key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size - max_single_key_size;
        second_dnx_qual_info_p->size =
            key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size - max_single_key_size;

        key_qual_map_p->size = max_single_key_size - (key_qual_map_p->lsb + offset_within_key);
        dnx_qual_info_p->size = max_single_key_size - (key_qual_map_p->lsb + offset_within_key);

        second_qual_map_p->lsb += key_qual_map_p->size;

        second_dnx_qual_info_p->offset += key_qual_map_p->size;

        if (dnx_qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {
            uint32 input_arg_uint32;
            input_arg_uint32 = dnx_qual_info_p->index;

            if (input_arg_uint32 != dnx_qual_info_p->index)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Input_arg %d mismatch with 32 bit unsigned integer %d.\r\n",
                             dnx_qual_info_p->index, input_arg_uint32);
            }
            if (key_qual_map_p->size >= SAL_UINT32_NOF_BITS)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "First split size %d is bigger than %d bits.\r\n",
                             key_qual_map_p->size, SAL_UINT32_NOF_BITS - 1);
            }

            dnx_qual_info_p->index = input_arg_uint32 & (SAL_UPTO_BIT(key_qual_map_p->size));
            second_dnx_qual_info_p->index = input_arg_uint32 >> (key_qual_map_p->size);
        }

        *was_split_p = TRUE;

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------SPLIT first qual name %s, first  key lsb %d first  qual_size %d first  lsb+offset %d \n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                     key_qual_map_p->size, (key_qual_map_p->lsb + offset_within_key));
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------SPLIT second qual name %s, second key lsb %d second qual_size %d second lsb+offset %d \n",
                     dnx_field_dnx_qual_text(unit, second_qual_map_p->qual_type), second_qual_map_p->lsb,
                     second_dnx_qual_info_p->size, (second_qual_map_p->lsb + offset_within_key));
    }

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
    dnx_field_qual_map_in_key_t key_qual_map;
    dnx_field_qual_map_in_key_t split_qual_map;
    dnx_field_key_attached_qual_info_t dnx_qual_info;
    dnx_field_key_attached_qual_info_t split_dnx_qual_info;
    dnx_field_stage_e field_stage;
    uint8 was_split = FALSE;
    uint32 key_index = 0, next_key_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    dnx_field_key_attached_qual_info_t_init(unit, &dnx_qual_info);
    dnx_field_key_attached_qual_info_t_init(unit, &split_dnx_qual_info);

    field_stage = key_in_info_p->field_stage;

    for (qual_index = 0;
         (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_in_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_index++)
    {
        sal_memcpy(&key_qual_map, &(key_in_info_p->key_template.key_qual_map[qual_index]), sizeof(key_qual_map));

        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, field_stage, context_id, key_qual_map.qual_type,
                                                    &key_in_info_p->qual_info[qual_index], &dnx_qual_info));

        was_split = FALSE;
        key_index = ((key_qual_map.lsb + key_id_p->offset_on_first_key) >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE)
            ? 1 : 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map.qual_type),
                     dnx_field_stage_text(unit, field_stage), (key_qual_map.lsb + key_id_p->offset_on_first_key),
                     key_index);

        if (key_index == 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SET: POTENTIAL SPLIT: for qual name %s, offset in key %d qual_size %d lsb+offset %d \n",
                         dnx_field_dnx_qual_text(unit, key_qual_map.qual_type),
                         key_qual_map.lsb, key_qual_map.size, (key_qual_map.lsb + key_id_p->offset_on_first_key));

            SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit,
                                                               key_id_p->offset_on_first_key,
                                                               &key_qual_map, &dnx_qual_info,
                                                               &split_qual_map, &split_dnx_qual_info, &was_split));
        }
        if (dnx_qual_info.input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {

            SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                            (unit, field_stage, context_id, &dnx_qual_info, &key_qual_map,
                             key_id_p->id[key_index], key_id_p->offset_on_first_key));
            if (was_split == TRUE)
            {
                next_key_index = key_index + 1;
                if (key_id_p->id[next_key_index] == DNX_FIELD_KEY_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no available key for split.\r\n");
                }
                SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                                (unit, field_stage, context_id, &split_dnx_qual_info, &split_qual_map,
                                 key_id_p->id[next_key_index], key_id_p->offset_on_first_key));
            }
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                              &key_qual_map, &dnx_qual_info,
                                              key_id_p->id[key_index],
                                              key_id_p->offset_on_first_key, FALSE, DNX_FIELD_FFC_ID_INVALID));

        if (was_split == TRUE)
        {
            next_key_index = key_index + 1;
            if (key_id_p->id[next_key_index] == DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is no available key for split.\r\n");
            }
            SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                                  &split_qual_map, &split_dnx_qual_info,
                                                  key_id_p->id[next_key_index],
                                                  key_id_p->offset_on_first_key, FALSE, DNX_FIELD_FFC_ID_INVALID));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "SET Second SPLIT qual name %s, field_stage %s, offset in key %d qual_size %d\n",
                         dnx_field_dnx_qual_text(unit, split_qual_map.qual_type), dnx_field_stage_text(unit,
                                                                                                       field_stage),
                         split_qual_map.lsb, split_dnx_qual_info.size);
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE, "offset %d,  %d size %d,  ffc input_type %d\n",
                     dnx_qual_info.offset, key_in_info_p->qual_info[qual_index].offset,
                     dnx_qual_info.size, dnx_qual_info.input_type);

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
                    (unit, flags, field_stage, fg_type, context_id, key_in_info_p->key_length, key_id_p));

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
            mdb_dt_key_id = DBAL_ENUM_FVAL_FIELD_KEY_A + dnx_data_field.map.ipmf1_key_select_get(unit);
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            mdb_dt_key_id = DBAL_ENUM_FVAL_FIELD_KEY_F + dnx_data_field.map.ipmf2_key_select_get(unit);
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            mdb_dt_key_id = DBAL_ENUM_FVAL_FIELD_KEY_A + dnx_data_field.map.ipmf3_key_select_get(unit);
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
    key_id_p->nof_bits[0] = dnx_data_field.map.key_size_get(unit);
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

shr_error_e
dnx_field_key_one_ffc_verify(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    int key_ndx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    if (key_in_info_p->field_stage == DNX_FIELD_STAGE_EPMF
        && key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
        {
            if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
            {
                uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC] = { 0 };

                SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                                (unit, key_in_info_p->field_stage, context_id, key_id_p->id[key_ndx], ffc_id));
                if (ffc_id[0] == DNX_FIELD_FFC_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "An ePMF lookup must have at least one FFC "
                                 "(at least one qualifier that is not const zero). "
                                 "Field group type %s, context %d stage %s.\n",
                                 dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id,
                                 dnx_field_stage_text(unit, key_in_info_p->field_stage));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

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
        case DNX_FIELD_GROUP_TYPE_KBP:
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

    SHR_INVOKE_VERIFY_DNX(dnx_field_key_one_ffc_verify(unit, context_id, key_in_info_p, key_id_p));

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
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_info_p,
    uint32 template_offset_within_key,
    uint32 *is_relative_p)
{

    uint32 qual_range_min;
    uint32 qual_range_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(ffc_instruction_info_p, _SHR_E_PARAM, "ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    qual_range_min = (key_qual_map_p->lsb + template_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    qual_range_max =
        (key_qual_map_p->lsb + template_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE +
        key_qual_map_p->size;

    *is_relative_p = FALSE;

    if ((ffc_instruction_info_p->key_offset >= qual_range_min) && (ffc_instruction_info_p->key_offset < qual_range_max))
    {
        *is_relative_p = TRUE;
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "qual_range_min %d qual_range_max %d ffc_instruction_info_p->key_offset %d key_qual_map_p->lsb %d\n",
                 qual_range_min, qual_range_max, ffc_instruction_info_p->key_offset, key_qual_map_p->lsb);

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
    dnx_field_ipmf2_qual_e ipmf2_base_qual;
    int mapped_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf2_ffc_instruction_info_p");
    SHR_NULL_CHECK(ipmf1_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf1_ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    *is_relative_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, inital_key_id, &ipmf2_base_qual));

    internal_ipmf2_qual_type = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &mapped_offset));

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
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    uint32 template_offset_within_key,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 *output_ffc_index_p)
{
    uint32 ffc_idx;
    uint32 is_relative = 0;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_instruction_info_t local_instruction;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    qualifier_ffc_info_p->qual_type = key_qual_map_p->qual_type;

    for (ffc_idx = 0;
         (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_idx] != DNX_FIELD_FFC_ID_INVALID);
         ffc_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                        (unit, field_stage, context_id, ffc_id[ffc_idx], &local_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_to_qual_is_relative
                        (unit, key_qual_map_p, &local_instruction, template_offset_within_key, &is_relative));

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
                         ffc_idx, ffc_id[ffc_idx], dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type),
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
    dnx_field_qual_map_in_key_t second_qual_map;
    dnx_field_key_attached_qual_info_t dummy_split_qual_info;
    dnx_field_key_attached_qual_info_t dummy_dnx_qual_info;
    uint32 key_index = 0;
    uint8 qual_was_split = FALSE;
    uint32 ffc_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    key_index =
        ((key_qual_map_p->lsb + key_id_p->offset_on_first_key) >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE) ? 1 : 0;

    if (key_index >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only %d keys supported.\r\n", DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                 dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), dnx_field_stage_text(unit, field_stage),
                 (key_qual_map_p->lsb + key_id_p->offset_on_first_key), key_index);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id, key_id_p->id[key_index],
                                                        key_qual_map_p, key_id_p->offset_on_first_key,
                                                        qualifier_ffc_info_p, &ffc_idx));

    if (key_index == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------GET: POTENTIAL SPLIT: for qual name %s, offset in key %d qual_size %d lsb+offset %d \n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                     key_qual_map_p->size, (key_qual_map_p->lsb + key_id_p->offset_on_first_key));

        SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit,
                                                           key_id_p->offset_on_first_key, key_qual_map_p,
                                                           &dummy_dnx_qual_info, &second_qual_map,
                                                           &dummy_split_qual_info, &qual_was_split));

        if (qual_was_split == TRUE)
        {

            if (key_index + 1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only %d keys supported, and qualifier is plit beyond that.\r\n",
                             DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX);
            }

            SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id,
                                                                key_id_p->id[key_index + 1],
                                                                &second_qual_map, key_id_p->offset_on_first_key,
                                                                qualifier_ffc_info_p, &ffc_idx));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------GET: The SECOND SPLIT: for qual name %s, offset in key %d qual_size %d ffc_idx %d \n",
                         dnx_field_dnx_qual_text(unit, second_qual_map.qual_type), second_qual_map.lsb,
                         second_qual_map.size, ffc_idx);

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

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_p, _SHR_E_PARAM, "ipmf2_ffc_p");
    SHR_NULL_CHECK(ipmf1_ffc_initial_p, _SHR_E_PARAM, "ipmf1_ffc_initial_p");

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    if (ipmf2_ffc_p->ffc_id != DNX_FIELD_FFC_ID_INVALID)
    {

        for (initial_key_id = DBAL_ENUM_FVAL_FIELD_KEY_F; initial_key_id <= DBAL_ENUM_FVAL_FIELD_KEY_J;
             initial_key_id++)
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
dnx_field_key_kbr_delete(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    uint32 qual_idx;
    uint32 ffc_idx = DNX_FIELD_FFC_ID_INVALID;
    dnx_field_context_t cascaded_from_context_id;
    int is_shared;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                     cascaded_from_context_id, EMPTY, EMPTY);
    }
    else
    {
        cascaded_from_context_id = context_id;
    }

    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG); qual_idx++)
    {

        for (ffc_idx = 0; (ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL) &&
             (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
              DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.key_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));

            if (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.ffc.ffc_id !=
                DNX_FIELD_FFC_ID_INVALID)
            {

                SHR_IF_ERR_EXIT(dnx_field_key_ffc_is_sharing_initial(unit, field_stage, context_id, group_ffc_info_p,
                                                                     &group_ffc_info_p->
                                                                     key.qualifier_ffc_info[qual_idx].
                                                                     ffc_info[ffc_idx].ffc_initial, &is_shared));

                LOG_DEBUG_EX(BSL_LOG_MODULE, "KBR DELETE -  context %d qual %s ffc %d is_shared %d\n",
                             context_id,
                             dnx_field_dnx_qual_text(unit,
                                                     group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type),
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                             ffc.ffc_id, is_shared);

                if (!is_shared)
                {
                    SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                                    (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                     group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                     ffc.key_id,
                                     group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                     ffc.ffc_id));
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_ffc_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    uint32 ffc_idx = 0;
    uint32 qual_idx;
    int is_shared;
    dnx_field_context_t cascaded_from_context_id;
    dnx_field_key_attached_qual_info_t dnx_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                     cascaded_from_context_id, EMPTY, EMPTY);

        for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG); qual_idx++)
        {
            dnx_field_qual_t qual_type = group_ffc_info_p->key.qualifier_ffc_info[qual_idx].qual_type;
            if (qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
            {
                continue;
            }

            for (ffc_idx = 0; (ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL) &&
                 (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.ffc.ffc_id !=
                  DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, field_stage, context_id, qual_type,
                                                            &key_in_info_p->qual_info[qual_idx], &dnx_qual_info));

                SHR_IF_ERR_EXIT(dnx_field_key_ffc_is_sharing_initial(unit, field_stage, context_id, group_ffc_info_p,
                                                                     &group_ffc_info_p->
                                                                     key.qualifier_ffc_info[qual_idx].
                                                                     ffc_info[ffc_idx].ffc_initial, &is_shared));

                LOG_DEBUG_EX(BSL_LOG_MODULE, "FFC DELETE -  stage %s qual %s context %d is_shared %d\n",
                             dnx_field_stage_text(unit, key_in_info_p->field_stage),
                             dnx_field_dnx_qual_text(unit, qual_type), context_id, is_shared);

                if (!is_shared)
                {

                    SHR_IF_ERR_EXIT(dnx_field_key_ipmf1_initial_key_occupation_bmp_ffc_deallocate
                                    (unit, cascaded_from_context_id,
                                     &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx].
                                       ffc_info[ffc_idx].ffc_initial)));

                    SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate
                                    (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                     group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                     ffc.ffc_id));

                    SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                                    (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                     group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                     ffc.ffc_id));
                }
            }
        }
    }

    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        for (ffc_idx = 0; ffc_idx < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
             DNX_FIELD_FFC_ID_INVALID; ffc_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));

            SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_kbr_ffc_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_group_ffc_info_t *group_ffc_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_ALLOC(group_ffc_info_p, sizeof(*group_ffc_info_p), "group_ffc_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_key_info_get
                    (unit, key_in_info_p->field_stage, context_id, key_in_info_p->fg_type, key_id_p,
                     &(key_in_info_p->key_template), group_ffc_info_p, &(key_in_info_p->app_db_id)));

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_delete
                    (unit, key_in_info_p->field_stage, context_id, key_in_info_p, group_ffc_info_p));

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_deallocate
                    (unit, key_in_info_p->field_stage, context_id, key_in_info_p, group_ffc_info_p));

exit:
    SHR_FREE(group_ffc_info_p);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_de_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                    (unit, key_in_info_p->field_stage, key_in_info_p->fg_type, context_id, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_tcam_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, key_in_info_p->field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                    (unit, key_in_info_p->fg_type, key_in_info_p->field_stage, key_in_info_p->key_length,
                     context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, FALSE));

    if (dnx_data_field.tests.epmf_debug_rely_on_tcam0_get(unit))
    {

        if (key_in_info_p->field_stage == DNX_FIELD_STAGE_EPMF
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
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_exem_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, key_in_info_p->field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                    (unit, key_in_info_p->fg_type, key_in_info_p->field_stage, key_in_info_p->key_length,
                     context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_mdb_dt_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                    (unit, key_in_info_p->field_stage, key_in_info_p->fg_type, context_id, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, FALSE));

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
    dnx_field_key_attach_info_in_t key_in_info;
    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_attach_info_in_t_init(unit, &key_in_info);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    key_in_info.field_stage = stage;
    sal_memcpy(&key_in_info.key_template, key_template_p, sizeof(key_in_info.key_template));

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, &key_in_info, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    switch (key_in_info_p->fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            SHR_IF_ERR_EXIT(dnx_field_key_tcam_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            SHR_IF_ERR_EXIT(dnx_field_key_de_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_EXEM:
            SHR_IF_ERR_EXIT(dnx_field_key_exem_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
            SHR_IF_ERR_EXIT(dnx_field_key_mdb_dt_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
            SHR_ERR_EXIT(_SHR_E_DISABLED,
                         "Should not get here. Probably an internal error. Stage %s FG type: type %s Detach from Context (%d)\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
            break;
        case DNX_FIELD_GROUP_TYPE_CONST:

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage %s FG type: type %s Detach from Context (%d) not supported. Unknown field group type.\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
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
dnx_field_key_qual_attach_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    uint32 qual_index;
    dnx_field_qual_class_e cur_class;
    dnx_field_input_type_e cur_input_type;
    uint32 qual_size;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_index++)
    {
        if (dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID)
        {
            cur_class = DNX_QUAL_CLASS(dnx_quals[qual_index]);
            cur_input_type = qual_info[qual_index].input_type;

            switch (cur_input_type)
            {
                case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
                case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_HEADER && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= DNX_FIELD_LAYER_NOF)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) for qual %s in FG %d context %d is out of range for input type LAYER. \n",
                                     qual_info[qual_index].input_arg, dnx_field_dnx_qual_text(unit,
                                                                                              dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    if (qual_info[qual_index].offset >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length
                        || qual_info[qual_index].offset < -dnx_data_field.stage.stage_info_get(unit,
                                                                                               field_stage)->pbus_header_length)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for qual %s in FG %d context %d is out of range (%d:%d) "
                                     "for input type LAYER for stage %s. \n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id,
                                     -dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                                     dnx_field_stage_text(unit, field_stage));
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_META_DATA:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_META && cur_class != DNX_FIELD_QUAL_CLASS_USER
                        && cur_class != DNX_FIELD_QUAL_CLASS_VW)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type metadata.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_quals[qual_index], &qual_size));
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && (qual_info[qual_index].offset + qual_size) >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                                     "stage %s (%d).\n",
                                     qual_info[qual_index].offset, qual_size, qual_info[qual_index].offset + qual_size,
                                     dnx_field_stage_text(unit, field_stage),
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_META_DATA2:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_META2 && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type), dnx_field_dnx_qual_text(unit,
                                                                                                        dnx_quals
                                                                                                        [qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type metadata2.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_quals[qual_index], &qual_size));
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && (qual_info[qual_index].offset + qual_size) >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_native_pbus)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                                     "stage %s (%d).\n",
                                     qual_info[qual_index].offset, qual_size, qual_info[qual_index].offset + qual_size,
                                     dnx_field_stage_text(unit, field_stage),
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_native_pbus);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
                case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_LAYER_RECORD && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= DNX_FIELD_LAYER_NOF)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) is out of range for input type LAYER_RECORDS. \n",
                                     qual_info[qual_index].input_arg);
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER &&
                        (qual_info[qual_index].offset > DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS ||
                         qual_info[qual_index].offset < 0))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for qual %s in FG %d context %d is out of range for input type LAYER_RECORDS (%d:%d). \n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, 0, DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_EXTERNAL:
                {
                    if ((cur_class != DNX_FIELD_QUAL_CLASS_USER))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type KBP.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_CASCADED:
                {
                    if ((cur_class != DNX_FIELD_QUAL_CLASS_USER))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= dnx_data_field.group.nof_fgs_get(unit))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) for qual %s in FG %d context %d is out of range for input type CASCADED - must be valid FG ID. \n",
                                     qual_info[qual_index].input_arg, dnx_field_dnx_qual_text(unit,
                                                                                              dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_CONST:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type), dnx_field_dnx_qual_text(unit,
                                                                                                        dnx_quals
                                                                                                        [qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }

                    if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s is not supported for External TCAM field groups (FG %d context %d). \n",
                                     dnx_field_input_type_text(cur_input_type), dnx_field_dnx_qual_text(unit,
                                                                                                        dnx_quals
                                                                                                        [qual_index]),
                                     fg_id, context_id);
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The input type(%d) supplied for qual %s in FG %d context %d is not valid. \n",
                                 cur_input_type, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]), fg_id,
                                 context_id);
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
