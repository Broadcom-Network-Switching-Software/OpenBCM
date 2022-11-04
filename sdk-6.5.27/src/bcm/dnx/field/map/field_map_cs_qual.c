/** \file field_map_cs_qual.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>

#include <bcm/field.h>

#include "field_map_local.h"
/*
 * }Include files
 */
extern const char *bcm_qual_description[bcmFieldQualifyCount];
extern const dnx_field_qual_map_t dnx_global_qual_map[bcmFieldQualifyCount];

shr_error_e
dnx_field_map_cs_qual_table_id(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * cs_table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_table_id_p, _SHR_E_PARAM, "cs_table_id_p");

    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_field_map_stage_info[stage].cs_table_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No context selection TCAM table id for stage:%s\n",
                     dnx_field_stage_text(unit, stage));
    }
    *cs_table_id_p = dnx_field_map_stage_info[stage].cs_table_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Return CS qual DBAL field
 * \param [in] unit                        - Device ID
 * \param [in] stage                      - The stage for which we check the context selection.
 * \param [in] context_id                 - The context. Relevant only for type conversions.
 * \param [in] bcm_qual                   - The qualifer
 * \param [in] cs_qual_index              - The input_arg.
 * \param [out] cs_dnx_qual_dbal_field_p  - The DBAL field of the qualifier.
 * \param [out] is_special_mapped_dnx_qual_p - If special mapping
 * \param [out] size_in_dbal_field_p      - If is_special_mapped_dnx_qual_p number of bits.
 * \param [out] offset_in_dbal_field_p    - If is_special_mapped_dnx_qual_p LSB in DBAL field.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_map_cs_qual_bcm_to_dnx_dbal(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    bcm_field_qualify_t bcm_qual,
    int cs_qual_index,
    dbal_fields_e * cs_dnx_qual_dbal_field_p,
    int *is_special_mapped_dnx_qual_p,
    int *size_in_dbal_field_p,
    int *offset_in_dbal_field_p)
{
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    const dnx_field_qual_map_t *lr_qual_map_entry_p;
    int is_supported;
    int is_special;
    int special_size;
    int special_offset;
    dnx_field_cs_qual_e cs_dnx_qual;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_dnx_qual_dbal_field_p, _SHR_E_PARAM, "cs_dnx_qual_p");

    /** Inializations to appease the compiler.*/
    (*size_in_dbal_field_p) = -1;
    (*offset_in_dbal_field_p) = -1;

    /*
     * Global mapping arrays are defined size of bcmFieldQualifyCount hence need to check that no qual is
     * equal (since bcm_field_qual_t start with 0) or bigger than  bcmFieldQualifyCount
     * */
    if (bcm_qual >= bcmFieldQualifyCount || bcm_qual < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal qualifier %d, for supported qualifiers, run: field qualifier ContextSelect stage=%s",
                     bcm_qual, dnx_field_stage_text(unit, stage));
    }

    lr_qual_map_entry_p = &dnx_global_qual_map[bcm_qual];

    cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];
    /*
     * If the qualifier is LR qualifier and not one of the general LR qualifier map it according to ForwardingLayerQualifier
     */
    if (DNX_QUAL_CLASS(lr_qual_map_entry_p->dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
    {
        if (DNX_QUAL_ID(lr_qual_map_entry_p->dnx_qual) != DNX_FIELD_LR_QUAL_PROTOCOL
            && DNX_QUAL_ID(lr_qual_map_entry_p->dnx_qual) != DNX_FIELD_LR_QUAL_OFFSET
            && DNX_QUAL_ID(lr_qual_map_entry_p->dnx_qual) != DNX_FIELD_LR_QUAL_QUALIFIER)
        {
            cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcmFieldQualifyForwardingLayerQualifier];
        }
    }

    if (cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb != NULL)
    {
        SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb
                        (unit, cs_qual_index, context_id, bcm_qual, &cs_dnx_qual));
    }
    else if (cs_qual_map_p->nof == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No DNX CS qualifier for bcmFieldQualify%s(%d) at index %d on stage:%s\n"
                     "To display the supported qualifiers, run: field qualifier ContextSelect stage=%s",
                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, cs_qual_index,
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, stage));
    }
    else if (cs_qual_map_p->nof == 1)
    {
        /*
         * If there is only one - it will be direct entry in the map
         */
        if (cs_qual_map_p->dnx_qual == DNX_FIELD_CS_QUAL_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
        }
        cs_dnx_qual = cs_qual_map_p->dnx_qual;
    }
    else if (cs_qual_map_p->nof > 1)
    {
        /*
         * There may be index shift between argument and actual array index
         */
        int shifted_index;
        shifted_index = cs_qual_index + cs_qual_map_p->index_shift;
        if ((shifted_index >= cs_qual_map_p->nof) || (shifted_index < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Shifted CS QUAL Index:%d (unshifted %d) is out of range:%d "
                         "for bcmFieldQualify%s\n on %s",
                         shifted_index, cs_qual_index, cs_qual_map_p->nof, dnx_field_bcm_qual_text(unit, bcm_qual),
                         dnx_field_stage_text(unit, stage));
        }
        /*
         * More than 1 - array of nof size should be provided
         */
        if (cs_qual_map_p->dnx_qual_array == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field Array for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
        }
        if (cs_qual_map_p->dnx_qual_array[shifted_index] == DNX_FIELD_CS_QUAL_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No DBAL field for bcmFieldQualify%s\n on stage:%s index %d shifted %d",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage),
                         cs_qual_index, shifted_index);
        }
        cs_dnx_qual = cs_qual_map_p->dnx_qual_array[shifted_index];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid nof %d bcmFieldQualify%s\n on stage:%s",
                     cs_qual_map_p->nof, dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_to_dbal
                    (unit, stage, cs_dnx_qual, &is_supported, cs_dnx_qual_dbal_field_p,
                     &is_special, &special_size, &special_offset));
    if (is_supported == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "CS qualifier for bcmFieldQualify%s(%d) on stage:%s index %d context %d "
                     "not avaialable for this device.\n"
                     "To display the supported qualifiers, run: field qualifier ContextSelect stage=%s",
                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, dnx_field_stage_text(unit, stage),
                     cs_qual_index, context_id, dnx_field_stage_text(unit, stage));
    }
    if (is_special)
    {
        (*is_special_mapped_dnx_qual_p) = TRUE;
        (*size_in_dbal_field_p) = special_size;
        (*offset_in_dbal_field_p) = special_offset;
    }
    else
    {
        (*is_special_mapped_dnx_qual_p) = FALSE;
    }

    /*
     * Block qualifers with conversion functions for bare metal.
     */
    if (cs_qual_map_p->conversion_cb != NULL)
    {
        uint8 is_std_1;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1 == FALSE)
        {
            int bare_metal_support;
            SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check(unit, cs_qual_map_p->conversion_cb,
                                                               &bare_metal_support));
            if (bare_metal_support == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier %s(%d) at index %d on stage:%s is not supported in BareMetal mode.\n",
                             dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, cs_qual_index,
                             dnx_field_stage_text(unit, stage));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_cs_qual_bcm_mapping_exists(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    uint8 *mapping_exists_p,
    uint8 *mapping_potentially_exists_for_device_p)
{
    dbal_fields_e qual_type_dbal_field;
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    int is_supported;
    int is_special;
    int size;
    int offset;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mapping_exists_p, _SHR_E_PARAM, "mapping_exists_p");
    SHR_NULL_CHECK(mapping_potentially_exists_for_device_p, _SHR_E_PARAM, "mapping_potentially_exists_for_device_p");

    *mapping_exists_p = FALSE;
    *mapping_potentially_exists_for_device_p = FALSE;
    cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];
    if (cs_qual_map_p->nof == 0)
    {
        /** No mapping. */
    }
    else if (cs_qual_map_p->nof == 1)
    {
        *mapping_exists_p = TRUE;
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_to_dbal
                        (unit, stage, cs_qual_map_p->dnx_qual, &is_supported, &qual_type_dbal_field, &is_special, &size,
                         &offset));
        if (is_supported)
        {
            *mapping_potentially_exists_for_device_p = TRUE;
        }
    }
    else if (cs_qual_map_p->nof > 1)
    {
        int index_in_array;
        *mapping_exists_p = TRUE;
        for (index_in_array = 0; index_in_array < cs_qual_map_p->nof; index_in_array++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_to_dbal
                            (unit, stage, cs_qual_map_p->dnx_qual_array[index_in_array], &is_supported,
                             &qual_type_dbal_field, &is_special, &size, &offset));
            if (is_supported)
            {
                *mapping_potentially_exists_for_device_p = TRUE;
                break;
            }

        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid nof value *%d) for BCM qual %d.\n", cs_qual_map_p->nof, bcm_qual);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_cs_qual_dnx_to_dbal(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_cs_qual_e dnx_qual,
    int *is_supported_p,
    dbal_fields_e * dbal_field_id_p,
    int *is_special_p,
    int *size_p,
    int *offset_p)
{
    dbal_fields_e regular_map_dbal_field;
    dbal_fields_e mapped_dbal_field;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_supported_p, _SHR_E_PARAM, "is_supported_p");

    /** Initilaization to appease the compiler.*/
    mapped_dbal_field = -1;

    regular_map_dbal_field = dnx_data_field_map.qual.cs_get(unit, field_stage, dnx_qual)->dbal_field;
    if (regular_map_dbal_field != DBAL_FIELD_EMPTY)
    {
        /** Regular mapping. */
        mapped_dbal_field = regular_map_dbal_field;
        (*is_special_p) = FALSE;
        (*is_supported_p) = TRUE;
    }
    else if (dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->size > 0)
    {
        /** Special mapping. */
        char *vw_name_p =
            dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->vw_name;

        mapped_dbal_field =
            dnx_data_field_map.qual.cs_get(unit, field_stage, DNX_FIELD_CS_QUAL_PEM_SCRATCH_PAD)->dbal_field;
        if (mapped_dbal_field == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "CS qualifier %d on stage %s has special mapping, "
                         "but stage does not support special mapping.\n",
                         dnx_qual, dnx_field_stage_text(unit, field_stage));
        }
        if (vw_name_p == NULL)
        {
            /** Offset by DNX DATA. */
            (*is_special_p) = TRUE;
            (*size_p) = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->size;
            (*offset_p) = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->offset;
            (*is_supported_p) = TRUE;
        }
        else
        {
            /** Offset by virtual wire. */
            VirtualWireInfo *vw_info;
            int nof_containers = dnx_data_field.virtual_wire.general_data_nof_containers_get(unit);
            int container_index;
            int vw_size;
            int vw_offset;
            int mapping_found;
            SHR_IF_ERR_EXIT(pemladrv_vw_wire_get
                            (unit, vw_name_p, dnx_field_map_stage_info[field_stage].pp_stage, &vw_info));
            if (vw_info->nof_mappings < 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                             "but it has %d nof_mappings.\n",
                             dnx_qual,
                             dnx_field_stage_text(unit, field_stage), MAX_VW_NAME_LENGTH, vw_name_p,
                             vw_info->nof_mappings);
            }
            if (vw_info->nof_mappings > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                             "but it has multiple (%d) mappings.\n",
                             dnx_qual,
                             dnx_field_stage_text(unit, field_stage), MAX_VW_NAME_LENGTH, vw_name_p,
                             vw_info->nof_mappings);
            }
            if (0 != sal_strncmp(vw_info->vw_mappings_arr[0].physical_wire_name, "general_data", MAX_VW_NAME_LENGTH))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                             "but it is mapped to signal \"%.*s\".\n",
                             dnx_qual,
                             dnx_field_stage_text(unit, field_stage), MAX_VW_NAME_LENGTH, vw_name_p,
                             MAX_VW_NAME_LENGTH, vw_info->vw_mappings_arr[0].physical_wire_name);
            }
            vw_size = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->size;
            vw_offset = vw_info->vw_mappings_arr[0].physical_wire_lsb;
            mapping_found = FALSE;
            for (container_index = 0; container_index < nof_containers; container_index++)
            {
                int offset_on_pem_scratch_pad =
                    dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, field_stage,
                                                                           container_index)->lsb_on_pem_scratch_pad;
                int offset_on_general_data = dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, field_stage,
                                                                                                    container_index)->
                    lsb_on_general_data;
                int nof_bits_on_container_on_pem_scratch_pad =
                    dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, field_stage,
                                                                           container_index)->nof_bits;
                if (vw_offset < offset_on_general_data)
                {
                    continue;
                }
                if ((vw_offset + vw_size) > (offset_on_general_data + nof_bits_on_container_on_pem_scratch_pad))
                {
                    continue;
                }
                (*is_special_p) = TRUE;
                (*size_p) = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, field_stage, dnx_qual)->size;
                (*offset_p) = offset_on_pem_scratch_pad + (vw_offset - offset_on_general_data);
                (*is_supported_p) = TRUE;
                mapping_found = TRUE;
                break;
            }
            if (mapping_found == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                             "placemeant on general data %d and %d. Cannot use for CS.\n",
                             dnx_qual,
                             dnx_field_stage_text(unit, field_stage), MAX_VW_NAME_LENGTH, vw_name_p,
                             vw_offset, vw_size);
            }
        }
    }
    else
    {
        (*is_supported_p) = FALSE;
    }

    if (*is_supported_p)
    {
        /** Sanity check. */
        if (mapped_dbal_field == -1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mapped_dbal_field not set.\n");
        }

        (*dbal_field_id_p) = mapped_dbal_field;
    }

    if (*is_supported_p)
    {
        dbal_tables_e cs_dbal_table_id;
        uint8 hw_supported;
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, field_stage, &cs_dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, cs_dbal_table_id, mapped_dbal_field, &hw_supported));
        if (hw_supported == FALSE)
        {
            if (*is_special_p)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "In stage %d cs qual %d is special but there is no PEM_SCRATCH_PAD support.\n",
                             field_stage, dnx_qual);
            }
            else
            {
                (*is_supported_p) = FALSE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_cs_qual_info_bcm_to_dnx(
    int unit,
    bcm_core_t core,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t context_id,
    bcm_field_presel_qualify_data_t * bcm_cs_qual_info,
    dnx_field_presel_qual_data_t * dnx_cs_qual_info)
{
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    const dnx_field_qual_map_t *lr_qual_map_p;
    int is_special_mapped_dnx_qual;
    int size_in_dbal_field;
    int offset_in_dbal_field;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    SHR_NULL_CHECK(bcm_cs_qual_info, _SHR_E_PARAM, "bcm_cs_qual_info");
    SHR_NULL_CHECK(dnx_cs_qual_info, _SHR_E_PARAM, "dnx_cs_qual_info");

    /** Initialization to appease the compiler.*/
    is_special_mapped_dnx_qual = -1;

    /*
     * First get dnx qualifiers
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_bcm_to_dnx_dbal(unit, dnx_stage, context_id,
                                                          bcm_cs_qual_info->qual_type,
                                                          bcm_cs_qual_info->qual_arg,
                                                          &(dnx_cs_qual_info->qual_type_dbal_field),
                                                          &is_special_mapped_dnx_qual,
                                                          &size_in_dbal_field, &offset_in_dbal_field));
    /** Sanity check.*/
    if (is_special_mapped_dnx_qual == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "is_special_mapped_dnx_qual not set.\n");
    }

    cs_qual_map_p = &dnx_field_map_stage_info[dnx_stage].cs_qual_map[bcm_cs_qual_info->qual_type];

    dnx_cs_qual_info->qual_mask[0] = bcm_cs_qual_info->qual_mask;
    if (cs_qual_map_p->conversion_cb == NULL)
    {
        /*
         * Now fill values, if it was user defined or no conversion callback on static on, just copy from bcm to dnx
         */
        dnx_cs_qual_info->qual_value[0] = bcm_cs_qual_info->qual_value;

        /*
         * If the qualifier is LR qualifier, use the relevant CB function
         */
        lr_qual_map_p = &dnx_global_qual_map[bcm_cs_qual_info->qual_type];
        if (DNX_QUAL_CLASS(lr_qual_map_p->dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
        {
            int lr_offset;
            int nof_mappings;
            uint32 qual_size;
            dnx_field_qual_t dnx_qual;
            uint32 qual_value;
            uint32 qual_mask;
            if (lr_qual_map_p->conversion_cb != NULL)
            {
                SHR_IF_ERR_EXIT(lr_qual_map_p->conversion_cb(unit, 0, core, &bcm_cs_qual_info->qual_value,
                                                             &(dnx_cs_qual_info->qual_value[0])));
            }

            /*
             * Restrict the size of the mask to the size of the qualifier.
             */
            dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, dnx_stage, DNX_QUAL_ID(lr_qual_map_p->dnx_qual));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, dnx_stage, dnx_qual, &nof_mappings, NULL, &qual_size));
            if (nof_mappings != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "LR qualifier is split. Unexpected. Not supported in context selection.\n");
            }
            if (qual_size > SAL_UINT32_NOF_BITS)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "LR qualifier size is %d, above %d. Unexpected. Not supported in context selection.\n",
                             qual_size, SAL_UINT32_NOF_BITS);
            }
            if (qual_size < SAL_UINT32_NOF_BITS)
            {
                dnx_cs_qual_info->qual_mask[0] &= SAL_UPTO_BIT(qual_size);
                if (utilex_bitstream_have_one_in_range(dnx_cs_qual_info->qual_value, qual_size,
                                                       WORDS2BITS(sizeof(dnx_cs_qual_info->qual_value) /
                                                                  sizeof(dnx_cs_qual_info->qual_value[0])) - 1))
                {
                    if (lr_qual_map_p->conversion_cb != NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "LR qualifier %s size in bits is %d, but given value is %d, converted to %d.\n",
                                     dnx_field_bcm_qual_text(unit, bcm_cs_qual_info->qual_type),
                                     qual_size, bcm_cs_qual_info->qual_value, dnx_cs_qual_info->qual_value[0]);
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "LR qualifier %s size in bits is %d, but given value is %d.\n",
                                     dnx_field_bcm_qual_text(unit, bcm_cs_qual_info->qual_type),
                                     qual_size, bcm_cs_qual_info->qual_value);
                    }
                }
            }
            /**
             * If the qualifier is LR qualifier, shift the value according to the qualifier offset
             */
            qual_value = dnx_cs_qual_info->qual_value[0];
            qual_mask = dnx_cs_qual_info->qual_mask[0];
            SHR_IF_ERR_EXIT(utilex_bitstream_clear(dnx_cs_qual_info->qual_value,
                                                   (sizeof(dnx_cs_qual_info->qual_value) /
                                                    sizeof(dnx_cs_qual_info->qual_value[0]))));
            SHR_IF_ERR_EXIT(utilex_bitstream_clear
                            (dnx_cs_qual_info->qual_mask,
                             (sizeof(dnx_cs_qual_info->qual_mask) / sizeof(dnx_cs_qual_info->qual_mask[0]))));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_lr_qual_offset_in_lr_qual(unit, dnx_stage, dnx_qual, &lr_offset));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_field(dnx_cs_qual_info->qual_value, lr_offset, qual_size, qual_value));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_field(dnx_cs_qual_info->qual_mask, lr_offset, qual_size, qual_mask));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(cs_qual_map_p->conversion_cb
                        (unit, 0, core, &bcm_cs_qual_info->qual_value, &(dnx_cs_qual_info->qual_value[0])));
    }

    /*
     * If the qualifier is special
     */
    if (is_special_mapped_dnx_qual)
    {
        uint32 qual_value;
        uint32 qual_mask;
        /** Sanity check. */
        if (size_in_dbal_field <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier size is %d\n", size_in_dbal_field);
        }
        /** Sanity check. */
        if ((offset_in_dbal_field < 0) || (offset_in_dbal_field >= SAL_UINT32_NOF_BITS))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier offset is %d\n", offset_in_dbal_field);
        }

        /*
         * Restrict the size of the mask to the size of the qualifier.
         */
        if (size_in_dbal_field > SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "qualifier size is %d, above %d. Unexpected. Not supported in context selection.\n",
                         size_in_dbal_field, SAL_UINT32_NOF_BITS);
        }
        if (size_in_dbal_field < SAL_UINT32_NOF_BITS)
        {
            dnx_cs_qual_info->qual_mask[0] &= SAL_UPTO_BIT(size_in_dbal_field);
            if (utilex_bitstream_have_one_in_range(dnx_cs_qual_info->qual_value, size_in_dbal_field,
                                                   WORDS2BITS(sizeof(dnx_cs_qual_info->qual_value) /
                                                              sizeof(dnx_cs_qual_info->qual_value[0])) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier %s size in bits is %d, but given value is %d.\n",
                             dnx_field_bcm_qual_text(unit, bcm_cs_qual_info->qual_type),
                             size_in_dbal_field, bcm_cs_qual_info->qual_value);
            }
        }
        /*
         * Shift the value.
         */
        qual_value = dnx_cs_qual_info->qual_value[0];
        qual_mask = dnx_cs_qual_info->qual_mask[0];
        SHR_IF_ERR_EXIT(utilex_bitstream_clear(dnx_cs_qual_info->qual_value,
                                               (sizeof(dnx_cs_qual_info->qual_value) /
                                                sizeof(dnx_cs_qual_info->qual_value[0]))));
        SHR_IF_ERR_EXIT(utilex_bitstream_clear
                        (dnx_cs_qual_info->qual_mask,
                         (sizeof(dnx_cs_qual_info->qual_mask) / sizeof(dnx_cs_qual_info->qual_mask[0]))));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                        (dnx_cs_qual_info->qual_value, offset_in_dbal_field, size_in_dbal_field, qual_value));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                        (dnx_cs_qual_info->qual_mask, offset_in_dbal_field, size_in_dbal_field, qual_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_cs_qual_dnx_dbal_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dbal_fields_e dnx_cs_qual_dbal_field,
    bcm_field_qualify_t * bcm_qual_p,
    uint32 *cs_qual_index_p,
    int *is_special_general_data_p)
{
    int bcm_qual;
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    dbal_fields_e dbal_field_cs_qual;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    SHR_NULL_CHECK(cs_qual_index_p, _SHR_E_PARAM, "cs_qual_index_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_cs_qual_dbal_field == DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field provided.\n");
    }
    if (dnx_cs_qual_dbal_field ==
        dnx_data_field_map.qual.cs_get(unit, stage, DNX_FIELD_CS_QUAL_PEM_SCRATCH_PAD)->dbal_field)
    {
        (*is_special_general_data_p) = TRUE;
        (*bcm_qual_p) = bcmFieldQualifyCount;
        SHR_EXIT();
    }
    else
    {
        (*is_special_general_data_p) = FALSE;
    }

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];

        if (cs_qual_map_p->nof == 0)
        {
            /*
             * BCM qualifier not mapped
             */
            continue;
        }
        else if (cs_qual_map_p->nof == 1)
        {
            dbal_field_cs_qual = dnx_data_field_map.qual.cs_get(unit, stage, cs_qual_map_p->dnx_qual)->dbal_field;
            if ((dbal_field_cs_qual == dnx_cs_qual_dbal_field) && BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map_p->flags))
            {
                if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                {
                    SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
                                    (unit, context_id, cs_qual_map_p->dnx_qual, bcm_qual_p, cs_qual_index_p));
                }
                else
                {
                    *bcm_qual_p = bcm_qual;
                    *cs_qual_index_p = 0;
                }
                break;
            }
        }
        else
        {
            int cs_qual_index;
            for (cs_qual_index = 0; cs_qual_index < cs_qual_map_p->nof; cs_qual_index++)
            {
                dbal_field_cs_qual =
                    dnx_data_field_map.qual.cs_get(unit, stage,
                                                   cs_qual_map_p->dnx_qual_array[cs_qual_index])->dbal_field;
                if ((dbal_field_cs_qual == dnx_cs_qual_dbal_field) && BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map_p->flags))
                {
                    if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                    {
                        SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
                                        (unit, context_id, cs_qual_map_p->dnx_qual_array[cs_qual_index],
                                         bcm_qual_p, cs_qual_index_p));
                    }
                    else
                    {
                        *bcm_qual_p = bcm_qual;
                        *cs_qual_index_p = cs_qual_index - cs_qual_map_p->index_shift;
                    }
                    break;
                }
            }
            if (cs_qual_index != cs_qual_map_p->nof)
            {
                break;
            }
        }
    }
    if (bcm_qual == bcmFieldQualifyCount)
    {
        /*
         * No mapping was found for this qualifier, set to default (Invalid) value
         */
        *bcm_qual_p = bcmFieldQualifyCount;
        SHR_ERR_EXIT(_SHR_E_PARAM, "No BCM qualifier mapping found for stage:%s DNX DBAL qualifier %d\n",
                     dnx_field_stage_text(unit, stage), dnx_cs_qual_dbal_field);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_cs_qual_special_qual_dbal_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dbal_fields_e dnx_cs_qual_dbal_field,
    uint32 dnx_value[DNX_DATA_MAX_FIELD_PRESELECTOR_NOF_UINT32_IN_CS_QUAL_HW],
    uint32 dnx_mask[DNX_DATA_MAX_FIELD_PRESELECTOR_NOF_UINT32_IN_CS_QUAL_HW],
    int *nof_bcm_quals_p,
    bcm_field_qualify_t bcm_quals[BCM_FIELD_MAX_NOF_CS_QUALIFIERS],
    uint32 cs_qual_indices[BCM_FIELD_MAX_NOF_CS_QUALIFIERS],
    uint32 bcm_values[BCM_FIELD_MAX_NOF_CS_QUALIFIERS],
    uint32 bcm_masks[BCM_FIELD_MAX_NOF_CS_QUALIFIERS])
{
    int bcm_qual;
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    dnx_field_cs_qual_e cs_dnx_qual_iter;
    dnx_field_cs_qual_e cs_dnx_quals[BCM_FIELD_MAX_NOF_CS_QUALIFIERS];
    int qual_idx;
    uint32 remaining_mask[DNX_DATA_MAX_FIELD_PRESELECTOR_NOF_UINT32_IN_CS_QUAL_HW];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_bcm_quals_p, _SHR_E_PARAM, "nof_bcm_quals_p");
    SHR_NULL_CHECK(bcm_quals, _SHR_E_PARAM, "bcm_quals");
    SHR_NULL_CHECK(cs_qual_indices, _SHR_E_PARAM, "cs_qual_indices");
    SHR_NULL_CHECK(bcm_values, _SHR_E_PARAM, "bcm_values");
    SHR_NULL_CHECK(bcm_masks, _SHR_E_PARAM, "bcm_masks");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_cs_qual_dbal_field == DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field provided.\n");
    }
    if (dnx_cs_qual_dbal_field !=
        dnx_data_field_map.qual.cs_get(unit, stage, DNX_FIELD_CS_QUAL_PEM_SCRATCH_PAD)->dbal_field)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Received DBAL field %s, must be %s\n",
                     dbal_field_to_string(unit, dnx_cs_qual_dbal_field),
                     dbal_field_to_string(unit,
                                          dnx_data_field_map.qual.cs_get(unit, stage,
                                                                         DNX_FIELD_CS_QUAL_PEM_SCRATCH_PAD)->dbal_field));
    }

    sal_memcpy(remaining_mask, dnx_mask, sizeof(dnx_mask[0]) * DNX_DATA_MAX_FIELD_PRESELECTOR_NOF_UINT32_IN_CS_QUAL_HW);

    (*nof_bcm_quals_p) = 0;

    for (cs_dnx_qual_iter = 0; cs_dnx_qual_iter < DNX_FIELD_CS_QUAL_NOF; cs_dnx_qual_iter++)
    {
        int size = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, stage, cs_dnx_qual_iter)->size;
        if (size > SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size (%d) of CS qual %d above %d.\n",
                         size, cs_dnx_qual_iter, SAL_UINT32_NOF_BITS);
        }
        if (size != 0)
        {
            int offset;
            uint32 qual_mask;
            char *vw_name_p =
                dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, stage, cs_dnx_qual_iter)->vw_name;
            if (vw_name_p == NULL)
            {
                /** Offset by DNX DATA. */
                offset =
                    dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, stage, cs_dnx_qual_iter)->offset;
            }
            else
            {
                /** Offset by virtual wire. */
                VirtualWireInfo *vw_info;
                int nof_containers = dnx_data_field.virtual_wire.general_data_nof_containers_get(unit);
                int container_index;
                int vw_size;
                int vw_offset;
                int offset_found;
                SHR_IF_ERR_EXIT(pemladrv_vw_wire_get
                                (unit, vw_name_p, dnx_field_map_stage_info[stage].pp_stage, &vw_info));
                if (vw_info->nof_mappings != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                                 "but it has %d nof_mappings.\n",
                                 cs_dnx_qual_iter,
                                 dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name_p,
                                 vw_info->nof_mappings);
                }
                if (0 !=
                    sal_strncmp(vw_info->vw_mappings_arr[0].physical_wire_name, "general_data", MAX_VW_NAME_LENGTH))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                                 "but it is mapped to signal \"%.*s\".\n",
                                 cs_dnx_qual_iter,
                                 dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name_p,
                                 MAX_VW_NAME_LENGTH, vw_info->vw_mappings_arr[0].physical_wire_name);
                }
                vw_size = dnx_data_field_map.qual.cs_special_general_data_qual_get(unit, stage, cs_dnx_qual_iter)->size;
                vw_offset = vw_info->vw_mappings_arr[0].physical_wire_lsb;
                offset_found = FALSE;
                for (container_index = 0; container_index < nof_containers; container_index++)
                {
                    int offset_on_pem_scratch_pad = dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, stage,
                                                                                                           container_index)->lsb_on_pem_scratch_pad;
                    int offset_on_general_data = dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, stage,
                                                                                                        container_index)->
                        lsb_on_general_data;
                    int nof_bits_on_container_on_pem_scratch_pad =
                        dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get(unit, stage,
                                                                               container_index)->nof_bits;
                    if (vw_offset < offset_on_general_data)
                    {
                        continue;
                    }
                    if ((vw_offset + vw_size) > (offset_on_general_data + nof_bits_on_container_on_pem_scratch_pad))
                    {
                        continue;
                    }

                    offset = offset_on_pem_scratch_pad + (vw_offset - offset_on_general_data);
                    offset_found = TRUE;
                    break;
                }
                if (offset_found == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "CS qualifier %d on stage %s is mapped to virtual wire \"%.*s\", "
                                 "but cannot find offset mapping.\n",
                                 cs_dnx_qual_iter, dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name_p);
                }
            }

            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(dnx_mask, offset, size, &qual_mask));
            if (qual_mask != 0)
            {
                cs_dnx_quals[*nof_bcm_quals_p] = cs_dnx_qual_iter;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(dnx_value, offset, size, &(bcm_values[*nof_bcm_quals_p])));
                SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(remaining_mask, offset, offset + size - 1));
                bcm_masks[*nof_bcm_quals_p] = qual_mask;
                (*nof_bcm_quals_p)++;
            }
        }
    }

    if (utilex_bitstream_have_one_in_range(remaining_mask, 0, WORDS2BITS(sizeof(remaining_mask) /
                                                                         sizeof(remaining_mask[0])) - 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Scratch pad had masked bits not in any special qualifer.\n");
    }

    for (qual_idx = 0; qual_idx < (*nof_bcm_quals_p); qual_idx++)
    {
        for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
        {
            cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];

            if (cs_qual_map_p->nof == 0)
            {
                /*
                 * BCM qualifier not mapped
                 */
                continue;
            }
            else if (cs_qual_map_p->nof == 1)
            {
                if (cs_qual_map_p->dnx_qual == cs_dnx_quals[qual_idx] && BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map_p->flags))
                {
                    if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                    {
                        SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
                                        (unit, context_id, cs_qual_map_p->dnx_qual,
                                         &(bcm_quals[qual_idx]), &(cs_qual_indices[qual_idx])));
                    }
                    else
                    {
                        bcm_quals[qual_idx] = bcm_qual;
                        cs_qual_indices[qual_idx] = 0;
                    }
                    break;
                }
            }
            else
            {
                int cs_qual_index;
                for (cs_qual_index = 0; cs_qual_index < cs_qual_map_p->nof; cs_qual_index++)
                {
                    if (cs_qual_map_p->dnx_qual_array[cs_qual_index] == cs_dnx_quals[qual_idx] &&
                        BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map_p->flags))
                    {
                        if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                        {
                            SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
                                            (unit, context_id, cs_qual_map_p->dnx_qual_array[cs_qual_index],
                                             &(bcm_quals[qual_idx]), &(cs_qual_indices[qual_idx])));
                        }
                        else
                        {
                            bcm_quals[qual_idx] = bcm_qual;
                            cs_qual_indices[qual_idx] = cs_qual_index - cs_qual_map_p->index_shift;
                        }
                        break;
                    }
                }
                if (cs_qual_index != cs_qual_map_p->nof)
                {
                    break;
                }
            }
        }
        if (bcm_qual == bcmFieldQualifyCount)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No BCM qualifier mapping found for stage:%s DNX qualifier %d\n",
                         dnx_field_stage_text(unit, stage), cs_dnx_quals[qual_idx]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_size_get(
    int unit,
    dnx_field_stage_e dnx_stage,
    dbal_fields_e dnx_qual_dbal_field,
    int *size)
{
    dbal_tables_e cs_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(size, _SHR_E_PARAM, "size");

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, dnx_stage, &cs_dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, cs_dbal_table_id, dnx_qual_dbal_field, TRUE, 0, INST_SINGLE, size));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_nof_lines(
    int unit,
    dnx_field_stage_e dnx_stage,
    uint32 *cs_nof_lines_p)
{
    uint32 cs_nof_lines;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    SHR_NULL_CHECK(cs_nof_lines_p, _SHR_E_PARAM, "cs_nof_lines_p");

    /*
     * Check the correct stage, and get its max presel ID
     */
    cs_nof_lines = dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_cs_lines;
    if (cs_nof_lines <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage:\"%s\" has no support for number of cs lines\n",
                     dnx_field_stage_text(unit, dnx_stage));
    }

    *cs_nof_lines_p = cs_nof_lines;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_init(
    int unit)
{
    bcm_field_qualify_t i_bcm_cs_qual;
    dnx_field_stage_e dnx_stage;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the context qualifiers map. */
    for (i_bcm_cs_qual = 0; i_bcm_cs_qual < bcmFieldQualifyCount; i_bcm_cs_qual++)
    {

        const dnx_field_cs_qual_map_t *cs_qual_map_p;

        DNX_FIELD_STAGE_CS_QUAL_ITERATOR(dnx_stage)
        {
            cs_qual_map_p = &dnx_field_map_stage_info[dnx_stage].cs_qual_map[i_bcm_cs_qual];

            /** Check if this bcm qual is mapped on this stage, if not continue. */
            if (cs_qual_map_p->dnx_qual == DNX_FIELD_CS_QUAL_INVALID)
            {
                continue;
            }
            if (ISEMPTY(bcm_qual_description[i_bcm_cs_qual]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "BCM CS Qualifier:\"%s\" mapped, but has no valid description",
                             dnx_field_bcm_qual_text(unit, i_bcm_cs_qual));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
