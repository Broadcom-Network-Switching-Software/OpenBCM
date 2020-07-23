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
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
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

shr_error_e
dnx_field_map_cs_qual_table_id(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * cs_table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_table_id_p, _SHR_E_PARAM, "cs_table_id_p");

    DNX_FIELD_STAGE_VERIFY(stage);

    if ((*cs_table_id_p = dnx_field_map_stage_info[stage].cs_table_id) == 0)
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
    const dnx_field_cs_qual_map_t *cs_qual_map_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(exists_p, _SHR_E_PARAM, "exists_p");

    DNX_FIELD_STAGE_VERIFY(stage);

    cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];
    if (cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb != NULL || cs_qual_map_p->nof > 0)
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
    const dnx_field_cs_qual_map_t *cs_qual_map_p;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_dnx_qual_p, _SHR_E_PARAM, "cs_dnx_qual_p");

    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual];
    if (cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb != NULL)
    {
        SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb
                        (unit, cs_qual_index, context_id, bcm_qual, cs_dnx_qual_p));
    }
    else if (cs_qual_map_p->nof == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No DNX CS qualifier for bcmFieldQualify%s(%d) at index %d on stage:%s\n"
                     "To display the supported qualifiers, run: field qualifier ContextSelect stage=%s",
                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, cs_qual_index, dnx_field_stage_text(unit,
                                                                                                            stage),
                     dnx_field_stage_text(unit, stage));
    }
    else if (cs_qual_map_p->nof == 1)
    {
        /*
         * If there is only one - it will be direct entry in the map
         */
        if ((*cs_dnx_qual_p = cs_qual_map_p->field_id) == DBAL_FIELD_EMPTY)
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
        cs_qual_index += cs_qual_map_p->index_shift;
        if ((cs_qual_index >= cs_qual_map_p->nof) || (cs_qual_index < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Shifted CS QUAL Index:%d is out of range:%d for bcmFieldQualify%s\n on %s",
                         cs_qual_index, cs_qual_map_p->nof, dnx_field_bcm_qual_text(unit, bcm_qual),
                         dnx_field_stage_text(unit, stage));
        }
        /*
         * More than 1 - array of nof size should be provided
         */
        if (cs_qual_map_p->field_id_array == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NULL DBAL FID Array for bcmFieldQualify%s\n on stage:%s",
                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage));
        }
        if ((*cs_dnx_qual_p = cs_qual_map_p->field_id_array[cs_qual_index]) == DBAL_FIELD_EMPTY)
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
    const dnx_field_cs_qual_map_t *cs_qual_map_p = NULL;
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

    cs_qual_map_p = &dnx_field_map_stage_info[dnx_stage].cs_qual_map[bcm_cs_qual_info->qual_type];
    /*
     * Now fill values, if it was user defined or no conversion callback on static on, just copy from bcm to dnx
     */
    if ((cs_qual_map_p == NULL) || (cs_qual_map_p->conversion_cb == NULL))
    {
        dnx_cs_qual_info->qual_value = bcm_cs_qual_info->qual_value;
    }
    else
    {
        SHR_IF_ERR_EXIT(cs_qual_map_p->conversion_cb
                        (unit, 0, core, &bcm_cs_qual_info->qual_value, &dnx_cs_qual_info->qual_value));
    }
    dnx_cs_qual_info->qual_mask = bcm_cs_qual_info->qual_mask;

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
    const dnx_field_cs_qual_map_t *cs_qual_map_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    SHR_NULL_CHECK(cs_qual_index_p, _SHR_E_PARAM, "cs_qual_index_p");
    DNX_FIELD_STAGE_VERIFY(stage);

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
            if ((cs_qual_map_p->field_id == dnx_cs_qual) && (cs_qual_map_p->flags & BCM_TO_DNX_BASIC_OBJECT))
            {
                if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                {
                    SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
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
            for (cs_qual_index = 0; cs_qual_index < cs_qual_map_p->nof; cs_qual_index++)
            {
                if ((cs_qual_map_p->field_id_array[cs_qual_index] == dnx_cs_qual)
                    && (cs_qual_map_p->flags & BCM_TO_DNX_BASIC_OBJECT))
                {
                    if (cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb != NULL)
                    {
                        SHR_IF_ERR_EXIT(cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb
                                        (unit, context_id, dnx_cs_qual, bcm_qual_p, cs_qual_index_p));
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
        SHR_ERR_EXIT(_SHR_E_PARAM, "No BCM qualifier mapping found for stage:%s DNX qualifier %d\n",
                     dnx_field_stage_text(unit, stage), dnx_cs_qual);
    }

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
    dnx_field_stage_e stage;
    rhhandle_t temp = NULL;
    cs_dnx_dump_t *cs_dnx_dump;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_dnx_dump_list_p, _SHR_E_PARAM, "cs_dnx_dump_list_p");

    if ((cs_dnx_dump_list = utilex_rhlist_create("cs_dnx_dump_list", sizeof(cs_dnx_dump_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create cs_dnx_list\n");
    }

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        cs_dnx_dump = NULL;
        DNX_FIELD_STAGE_CS_QUAL_ITERATOR(stage)
        {
            if (dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual].nof != 0)
            {
                if (cs_dnx_dump == NULL)
                {
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(cs_dnx_dump_list, "", RHID_TO_BE_GENERATED, &temp));

                    cs_dnx_dump = (cs_dnx_dump_t *) temp;
                    cs_dnx_dump->bcm_qual = bcm_qual;
                }
                if (dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual].nof == 1)
                {
                    cs_dnx_dump->field_id[stage] = dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual].field_id;
                }
                else
                {
                    cs_dnx_dump->field_id[stage] =
                        dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual].field_id_array[0];
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
dnx_field_map_cs_qual_dnx_get(
    int unit,
    dnx_field_stage_e stage,
    const dbal_fields_e ** dnx_cs_qual_info_p,
    int *dnx_cs_qual_nof_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_NULL_CHECK(dnx_cs_qual_info_p, _SHR_E_PARAM, "dnx_cs_qual_info_p");
    SHR_NULL_CHECK(dnx_cs_qual_nof_p, _SHR_E_PARAM, "dnx_cs_qual_nof_p");

    *dnx_cs_qual_info_p = dnx_field_map_stage_info[stage].cs_qual_info;
    *dnx_cs_qual_nof_p = dnx_field_map_stage_info[stage].cs_qual_nof;

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

    /*
     * Verify that the size of CS qualifiers does not exceed the maximum for all stages.
     */
    DNX_FIELD_STAGE_CS_QUAL_ITERATOR(dnx_stage)
    {
        if (dnx_field_map_stage_info[dnx_stage].cs_qual_nof > DNX_FIELD_CS_QUAL_NOF_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage:\"%s\" has %d CS qualifiers. Please update the maximum of all stages "
                         "which is currently %d.\n",
                         dnx_field_stage_text(unit, dnx_stage),
                         dnx_field_map_stage_info[dnx_stage].cs_qual_nof, DNX_FIELD_CS_QUAL_NOF_MAX);
        }
    }

    /** Verify the context qualifiers map. */
    for (i_bcm_cs_qual = 0; i_bcm_cs_qual < bcmFieldQualifyCount; i_bcm_cs_qual++)
    {
        dbal_fields_e cs_qual_dbal_field;
        const dnx_field_cs_qual_map_t *cs_qual_map;

        DNX_FIELD_STAGE_CS_QUAL_ITERATOR(dnx_stage)
        {
            cs_qual_map = &dnx_field_map_stage_info[dnx_stage].cs_qual_map[i_bcm_cs_qual];

            /** Check if this bcm qual is mapped on this stage, if not continue. */
            if ((cs_qual_dbal_field = cs_qual_map->field_id) == 0)
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
