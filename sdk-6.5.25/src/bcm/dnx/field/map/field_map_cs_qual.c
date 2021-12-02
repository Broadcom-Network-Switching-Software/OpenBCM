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
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/sand/sand_signals.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <bcm/field.h>

#include "field_map_local.h"
/*
 * }Include files
 */
extern const char *bcm_qual_description[bcmFieldQualifyCount];

shr_error_e
dnx_field_map_cs_qual_table_id(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * cs_table_id_p)
{
    dnx_field_map_stage_info_t field_map_stage_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_table_id_p, _SHR_E_PARAM, "cs_table_id_p");

    DNX_FIELD_STAGE_VERIFY(stage);
    SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, stage, &field_map_stage_info));

    if ((*cs_table_id_p = field_map_stage_info.cs_table_id) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No context selection TCAM table id for stage:%s\n",
                     dnx_field_stage_text(unit, stage));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_bcm_mapping_exists(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    int *exists_p)
{
    dnx_field_cs_qual_map_t cs_qual_map;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(exists_p, _SHR_E_PARAM, "exists_p");

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, stage, bcm_qual, &cs_qual_map));
    if (cs_qual_map.qual_type_bcm_to_dnx_conversion_cb != NULL || cs_qual_map.nof > 0)
    {
        (*exists_p) = TRUE;
    }
    else
    {
        (*exists_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_bcm_to_dnx(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    bcm_field_qualify_t bcm_qual,
    int cs_qual_index,
    dbal_fields_e * cs_dnx_qual_p)
{
    dnx_field_cs_qual_map_t cs_qual_map;
    dnx_field_qual_map_t lr_qual_map_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_dnx_qual_p, _SHR_E_PARAM, "cs_dnx_qual_p");

    SHR_IF_ERR_EXIT(dnx_field_map_global_qual_map_get_dispatch(unit, bcm_qual, &lr_qual_map_entry));
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, stage, bcm_qual, &cs_qual_map));

    /*
     * If the qualifier is LR qualifier and not one of the general LR qualifier map it according to ForwardingLayerQualifier
     */
    if (DNX_QUAL_CLASS(lr_qual_map_entry.dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
    {
        if (DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_PROTOCOL
            && DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_OFFSET
            && DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_QUALIFIER)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch
                            (unit, stage, bcmFieldQualifyForwardingLayerQualifier, &cs_qual_map));
        }
    }

    if (cs_qual_map.qual_type_bcm_to_dnx_conversion_cb != NULL)
    {
        SHR_IF_ERR_EXIT(cs_qual_map.qual_type_bcm_to_dnx_conversion_cb
                        (unit, cs_qual_index, context_id, bcm_qual, cs_dnx_qual_p));
    }
    else if (cs_qual_map.nof == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No DNX CS qualifier for bcmFieldQualify%s(%d) at index %d on stage:%s\n"
                     "To display the supported qualifiers, run: field qualifier ContextSelect stage=%s",
                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, cs_qual_index, dnx_field_stage_text(unit,
                                                                                                            stage),
                     dnx_field_stage_text(unit, stage));
    }
    else if (cs_qual_map.nof == 1)
    {
        /*
         * If there is only one - it will be direct entry in the map
         */
        if ((*cs_dnx_qual_p = cs_qual_map.field_id) == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NULL DBAL FID for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
        }
    }
    else
    {
        /*
         * There may be index shift between argument and actual array index
         */
        cs_qual_index += cs_qual_map.index_shift;
        if ((cs_qual_index >= cs_qual_map.nof) || (cs_qual_index < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Shifted CS QUAL Index:%d is out of range:%d for bcmFieldQualify%s\n on %s",
                         cs_qual_index, cs_qual_map.nof, dnx_field_bcm_qual_text(unit, bcm_qual),
                         dnx_field_stage_text(unit, stage));
        }
        /*
         * More than 1 - array of nof size should be provided
         */
        if (cs_qual_map.field_id_array == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NULL DBAL FID Array for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
        }
        if ((*cs_dnx_qual_p = cs_qual_map.field_id_array[cs_qual_index]) == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NULL DBAL FID for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
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
    dnx_field_cs_qual_map_t cs_qual_map;
    dnx_field_qual_map_t lr_qual_map;
    dnx_field_layer_record_qual_info_t lr_qual_map_info;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    SHR_NULL_CHECK(bcm_cs_qual_info, _SHR_E_PARAM, "bcm_cs_qual_info");
    SHR_NULL_CHECK(dnx_cs_qual_info, _SHR_E_PARAM, "dnx_cs_qual_info");
    /*
     * First get dnx qualifiers
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_bcm_to_dnx(unit, dnx_stage, context_id,
                                                     bcm_cs_qual_info->qual_type,
                                                     bcm_cs_qual_info->qual_arg,
                                                     &(dnx_cs_qual_info->qual_type_dbal_field)));

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, dnx_stage, bcm_cs_qual_info->qual_type, &cs_qual_map));

    dnx_cs_qual_info->qual_mask = bcm_cs_qual_info->qual_mask;
    if (cs_qual_map.conversion_cb == NULL)
    {
        /*
         * Now fill values, if it was user defined or no conversion callback on static on, just copy from bcm to dnx
         */
        dnx_cs_qual_info->qual_value = bcm_cs_qual_info->qual_value;

        /*
         * If the qualifier is LR qualifier, use the relevant CB function
         */
        SHR_IF_ERR_EXIT(dnx_field_map_global_qual_map_get_dispatch(unit, bcm_cs_qual_info->qual_type, &lr_qual_map));
        if (DNX_QUAL_CLASS(lr_qual_map.dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
        {
            int lr_offset;
            dbal_fields_e lr_qual_field_id;
            if (lr_qual_map.conversion_cb != NULL)
            {
                SHR_IF_ERR_EXIT(lr_qual_map.conversion_cb(unit, 0, core, &bcm_cs_qual_info->qual_value,
                                                          &dnx_cs_qual_info->qual_value));
            }

            /*
             * Get the raw value according to dbal field
             */
            SHR_IF_ERR_EXIT(dnx_field_map_layer_record_qual_info_get_dispatch
                            (unit, dnx_stage, DNX_QUAL_ID(lr_qual_map.dnx_qual), &lr_qual_map_info));
            lr_qual_field_id = lr_qual_map_info.dbal_field_id;
            if (dbal_fields_is_field_encoded(unit, lr_qual_field_id))
            {

                SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                                (unit, lr_qual_field_id, dnx_cs_qual_info->qual_value,
                                 &(dnx_cs_qual_info->qual_value)));
            }

            /**
             * If the qualifier is LR qualifier, shift the value according to the qualifier offset
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_lr_qual_offset_in_lr_qual
                            (unit, dnx_stage, DNX_QUAL_ID(lr_qual_map.dnx_qual), &lr_offset));
            dnx_cs_qual_info->qual_value <<= lr_offset;
            dnx_cs_qual_info->qual_mask <<= lr_offset;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(cs_qual_map.conversion_cb
                        (unit, 0, core, &bcm_cs_qual_info->qual_value, &dnx_cs_qual_info->qual_value));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_dnx_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dbal_fields_e dnx_cs_qual,
    bcm_field_qualify_t * bcm_qual_p,
    uint32 *cs_qual_index_p)
{
    int bcm_qual;
    dnx_field_cs_qual_map_t cs_qual_map;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    SHR_NULL_CHECK(cs_qual_index_p, _SHR_E_PARAM, "cs_qual_index_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, stage, bcm_qual, &cs_qual_map));
        if (cs_qual_map.nof == 0)
        {
            /*
             * BCM qualifier not mapped
             */
            continue;
        }
        else if (cs_qual_map.nof == 1)
        {
            if ((cs_qual_map.field_id == dnx_cs_qual) && BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map.flags))
            {
                if (cs_qual_map.qual_type_dnx_to_bcm_conversion_cb != NULL)
                {
                    SHR_IF_ERR_EXIT(cs_qual_map.qual_type_dnx_to_bcm_conversion_cb
                                    (unit, context_id, dnx_cs_qual, bcm_qual_p, cs_qual_index_p));
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
            for (cs_qual_index = 0; cs_qual_index < cs_qual_map.nof; cs_qual_index++)
            {
                if ((cs_qual_map.field_id_array[cs_qual_index] == dnx_cs_qual)
                    && BCM_TO_DNX_IS_BAISC_OBJ(cs_qual_map.flags))
                {
                    if (cs_qual_map.qual_type_dnx_to_bcm_conversion_cb != NULL)
                    {
                        SHR_IF_ERR_EXIT(cs_qual_map.qual_type_dnx_to_bcm_conversion_cb
                                        (unit, context_id, dnx_cs_qual, bcm_qual_p, cs_qual_index_p));
                    }
                    else
                    {
                        *bcm_qual_p = bcm_qual;
                        *cs_qual_index_p = cs_qual_index - cs_qual_map.index_shift;
                    }
                    break;
                }
            }
            if (cs_qual_index != cs_qual_map.nof)
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
        SHR_ERR_EXIT(_SHR_E_PARAM, "No BCM qualifier mapping found for stage:%s DNX qualifier %d\n",
                     dnx_field_stage_text(unit, stage), dnx_cs_qual);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_cs_qual_size_get(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_qualify_t bcm_qual,
    int cs_qual_index,
    int *size)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e cs_dbal_table_id;
    dbal_table_field_info_t *dbal_table_field_info;
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(size, _SHR_E_PARAM, "size");
    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    rv = dnx_field_map_cs_qual_bcm_to_dnx(unit, dnx_stage, 0, bcm_qual, cs_qual_index, &dbal_field_id);
    if (rv != _SHR_E_NONE)
    {
        *size = 0;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, dnx_stage, &cs_dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal
                    (unit, cs_dbal_table_id, dbal_field_id, TRUE, 0, INST_SINGLE, &dbal_table_field_info,
                     &field_index_in_table, &parent_field_id));

    *size = dbal_table_field_info->field_nof_bits;

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_field_map_cs_qual_dump(
    int unit,
    rhlist_t ** cs_dnx_dump_list_p)
{
    rhlist_t *cs_dnx_dump_list = NULL;
    bcm_field_qualify_t bcm_qual;
    bcm_field_qualify_t cs_bcm_qual;
    dnx_field_qual_map_t lr_qual_map_entry;
    dnx_field_cs_qual_map_t cs_qual_map;
    dnx_field_stage_e stage;
    rhhandle_t temp = NULL;
    cs_dnx_dump_t *cs_dnx_dump;
    dnx_field_map_stage_info_t field_map_stage_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_dnx_dump_list_p, _SHR_E_PARAM, "cs_dnx_dump_list_p");

    if ((cs_dnx_dump_list = utilex_rhlist_create("cs_dnx_dump_list", sizeof(cs_dnx_dump_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create cs_dnx_list\n");
    }

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        cs_dnx_dump = NULL;
        DNX_FIELD_STAGE_CS_QUAL_ITERATOR(stage, field_map_stage_info)
        {
            cs_bcm_qual = bcm_qual;
            /*
             * If the qualifier is LR qualifier and not one of the general LR qualifier map it according to ForwardingLayerQualifier
             */
            SHR_IF_ERR_EXIT(dnx_field_map_global_qual_map_get_dispatch(unit, bcm_qual, &lr_qual_map_entry));
            if (DNX_QUAL_CLASS(lr_qual_map_entry.dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
            {
                if (DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_PROTOCOL
                    && DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_OFFSET
                    && DNX_QUAL_ID(lr_qual_map_entry.dnx_qual) != DNX_FIELD_LR_QUAL_QUALIFIER)
                {
                    cs_bcm_qual = bcmFieldQualifyForwardingLayerQualifier;
                }
            }
            SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, stage, cs_bcm_qual, &cs_qual_map));
            if (cs_qual_map.nof != 0)
            {
                if (cs_dnx_dump == NULL)
                {
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(cs_dnx_dump_list, "", RHID_TO_BE_GENERATED, &temp));

                    cs_dnx_dump = (cs_dnx_dump_t *) temp;
                    cs_dnx_dump->bcm_qual = bcm_qual;
                }
                if (cs_qual_map.nof == 1)
                {
                    cs_dnx_dump->field_id[stage] = cs_qual_map.field_id;
                }
                else
                {
                    cs_dnx_dump->field_id[stage] = cs_qual_map.field_id_array[0];
                }
            }
        }
    }

    *cs_dnx_dump_list_p = cs_dnx_dump_list;
exit:
    if (SHR_FUNC_ERR() && (cs_dnx_dump_list != NULL))
    {
        utilex_rhlist_free_all(cs_dnx_dump_list);
    }
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
    dnx_field_map_stage_info_t field_map_stage_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the context qualifiers map. */
    for (i_bcm_cs_qual = 0; i_bcm_cs_qual < bcmFieldQualifyCount; i_bcm_cs_qual++)
    {
        dbal_fields_e cs_qual_dbal_field;
        dnx_field_cs_qual_map_t cs_qual_map;

        DNX_FIELD_STAGE_CS_QUAL_ITERATOR(dnx_stage, field_map_stage_info)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get_dispatch(unit, dnx_stage, i_bcm_cs_qual, &cs_qual_map));

            /** Check if this bcm qual is mapped on this stage, if not continue. */
            if ((cs_qual_dbal_field = cs_qual_map.field_id) == 0)
            {
                continue;
            }
            if (ISEMPTY(bcm_qual_description[i_bcm_cs_qual]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "BCM CS Qualifier:\"%s\" mapped to:\"%s\" dbal field, has no valid description",
                             dnx_field_bcm_qual_text(unit, i_bcm_cs_qual), dbal_field_to_string(unit,
                                                                                                cs_qual_dbal_field));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
