/** \file field_context.c
 * 
 *
 * Field Context procedures for DNX.
 *
 * Will hold the first procedures that the dispatcher invoke (User API's).
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
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_key.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.h>
#include "field_utils.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <src/bcm/dnx/stat/crps/crps_engine.h>
#include <soc/dnx/dbal/dbal_dynamic.h>

#if defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#endif

/*
 * }
 */

#define DNX_FIELD_CONTEXT_IPMF1_INFO dnx_field_context_sw.context_info.context_ipmf1_info
#define DNX_FIELD_CONTEXT_IPMF2_INFO dnx_field_context_sw.context_info.context_ipmf2_info
#define DNX_FIELD_CONTEXT_STATE_TABLE_INFO dnx_field_context_sw.context_info.state_table_info

/**Takes*/
typedef enum
{
    DNX_FIELD_CONTEXT_FEATURE_COMPARE,
    DNX_FIELD_CONTEXT_FEATURE_HASHING,
    DNX_FIELD_CONTEXT_FEATURE_SCE,
} dnx_field_context_feature_e;

static shr_error_e
dnx_field_context_key_info_t_init(
    int unit,
    dnx_field_context_key_info_t * key_info_p)
{
    int qual_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_info_p, _SHR_E_PARAM, "key_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, &key_info_p->key_id));
    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &key_info_p->key_template));
    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &key_info_p->attach_info[qual_index]));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_context_compare_mode_info_t_init(
    int unit,
    dnx_field_context_compare_mode_info_t * mode_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(mode_info_p, 0x0, sizeof(*mode_info_p));

    mode_info_p->mode = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
    mode_info_p->is_set = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_context_key_info_t_init
                    (unit, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]));
    SHR_IF_ERR_EXIT(dnx_field_context_key_info_t_init
                    (unit, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY]));
exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_compare_info_t_init(
    int unit,
    dnx_field_context_compare_info_t * compare_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(compare_info_p, _SHR_E_PARAM, "compare_info_p");

    sal_memset(compare_info_p, 0x0, sizeof(*compare_info_p));

    SHR_IF_ERR_EXIT(dnx_field_context_compare_mode_info_t_init(unit, &compare_info_p->pair_1));
    SHR_IF_ERR_EXIT(dnx_field_context_compare_mode_info_t_init(unit, &compare_info_p->pair_2));
exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_hashing_info_t_init(
    int unit,
    dnx_field_context_hashing_info_t * hash_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(hash_info_p, _SHR_E_PARAM, "hash_info_p");

    sal_memset(hash_info_p, 0x0, sizeof(*hash_info_p));

    hash_info_p->mode = DNX_FIELD_CONTEXT_HASH_MODE_DISABLED;   /* Hashing Disabled */
    hash_info_p->is_set = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_context_key_info_t_init(unit, &hash_info_p->key_info));
exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_state_table_sw_info_t_init(
    int unit,
    dnx_field_context_state_table_sw_info_t * state_table_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state_table_info_p, _SHR_E_PARAM, "state_table_info_p");

    sal_memset(state_table_info_p, 0x0, sizeof(*state_table_info_p));

    state_table_info_p->mode = DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED;     /* State Table Disabled */
    state_table_info_p->is_set = FALSE;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initializes a dnx_field_context_ipmf1_sw_info_t structure used for the SW state of an iPMF1 context.
 * \param [in] unit                  - Device id
 * \param [in] context_ipmf1_info_p  - Structure to initialize.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_context_ipmf1_sw_info_t_init(
    int unit,
    dnx_field_context_ipmf1_sw_info_t * context_ipmf1_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_ipmf1_info_p, _SHR_E_PARAM, "context_ipmf1_info_p");

    sal_memset(context_ipmf1_info_p, 0x0, sizeof(*context_ipmf1_info_p));

    SHR_IF_ERR_EXIT(dnx_field_context_compare_info_t_init(unit, &(context_ipmf1_info_p->compare_info)));
    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_t_init(unit, &(context_ipmf1_info_p->hashing_info)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initializes a dnx_field_context_ipmf2_sw_info_t structure used for the SW state of an iPMF2 context.
 * \param [in] unit                  - Device id
 * \param [in] context_ipmf2_info_p  - Structure to initialize.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_context_ipmf2_sw_info_t_init(
    int unit,
    dnx_field_context_ipmf2_sw_info_t * context_ipmf2_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_ipmf2_info_p, _SHR_E_PARAM, "context_ipmf2_info_p");

    sal_memset(context_ipmf2_info_p, 0x0, sizeof(*context_ipmf2_info_p));

    context_ipmf2_info_p->cascaded_from = DNX_FIELD_CONTEXT_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_mode_t_init(
    int unit,
    dnx_field_context_mode_t * context_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_mode_p, _SHR_E_PARAM, "context_mode_p");

    sal_memset(context_mode_p, 0x0, sizeof(*context_mode_p));

    context_mode_p->context_ipmf1_mode.compare_mode_1 = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
    context_mode_p->context_ipmf1_mode.compare_mode_2 = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
    context_mode_p->context_ipmf1_mode.hash_mode = DNX_FIELD_CONTEXT_HASH_MODE_DISABLED;
    context_mode_p->state_table_mode = DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED;
    context_mode_p->state_table_engine_mode = DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED;
    context_mode_p->context_ipmf2_mode.cascaded_from = DNX_FIELD_CONTEXT_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_apptype_info_t_init(
    int unit,
    dnx_field_context_apptype_info_t * apptype_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(apptype_info_p, _SHR_E_PARAM, "apptype_info_p");

    sal_memset(apptype_info_p, 0, sizeof(*apptype_info_p));

    apptype_info_p->base_static_apptype = DNX_FIELD_APPTYPE_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_apptype_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_field.external_tcam.apptype_user_1st_get(unit) < bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First user defined Apptype %d, last predefined Apptype %d.\r\n",
                     dnx_data_field.external_tcam.apptype_user_1st_get(unit), bcmFieldAppTypeCount - 1);
    }

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Print compare mode string()
* \param [in] compare_mode         - compare mode type (look at dnx_field_context_compare_mode_e)
* \return
*   char * - string of compare mode
* \remark
*   * None
* \see
*   * None
*/
static char *
dnx_field_context_compare_text(
    dnx_field_context_compare_mode_e compare_mode)
{
    char *str = "";

    if ((compare_mode < DNX_FIELD_CONTEXT_COMPARE_MODE_FIRST) || (compare_mode >= DNX_FIELD_CONTEXT_COMPARE_MODE_NOF))
    {
        str = "Invalid compare mode";
    }
    else if (compare_mode == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        str = "None";
    }
    else if (compare_mode == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE)
    {
        str = "Single";
    }
    else if (compare_mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
    {
        str = "Double";
    }

    return str;
}

/**
* \brief
*  Verify parameters of dnx_field_context_create()
*
* \param [in] unit                - Device ID
* \param [in] flags               - Flags for context creation (WITH_ID)
* \param [in] stage               - Field Stage to create context for
* \param [in] context_mode_p      - context properties struct look at (dnx_field_context_mode_t)
* \param [in] field_context_id_p  - pointer to Context ID, to verify it is legal.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_create_verify(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_mode_t * context_mode_p,
    dnx_field_context_t * field_context_id_p)
{
    dnx_field_context_t cascaded_from;
    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_mode_p, _SHR_E_PARAM, "context_mode_p");
    SHR_NULL_CHECK(field_context_id_p, _SHR_E_PARAM, "field_context_id_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    /*
     * Verify that the stage is a PMF stage.
     */
    if (stage == DNX_FIELD_STAGE_ACE || stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage not a PMF stage, received stage \"%s\".\r\n",
                     dnx_field_stage_text(unit, stage));
    }

    if (flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID)
    {
        DNX_FIELD_CONTEXT_ID_VERIFY(unit, *field_context_id_p);
        /*
         * If somebody wants to create the default context, provide a more informative error
         * than to just say that it is already allocated.
         * Still verify that it is allocated, so that we won't get an error message when creating the default context.
         */
        if ((*field_context_id_p) == DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                            (unit, stage, DNX_FIELD_CONTEXT_ID_DEFAULT(unit), &is_alloc));
            if (is_alloc)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID %d is the default context, cannot be allocated.\r\n",
                             DNX_FIELD_CONTEXT_ID_DEFAULT(unit));
            }
        }
    }

    /*
     * Verify the context mode according to the field stage
     */
    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * Verify that the compare mode is legal.
         */
        if (context_mode_p->context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE &&
            context_mode_p->context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE &&
            context_mode_p->context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported compare mode for first pair: %d\r\n.",
                         context_mode_p->context_ipmf1_mode.compare_mode_1);
        }
        if (context_mode_p->context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE &&
            context_mode_p->context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE &&
            context_mode_p->context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported compare mode for second pair: %d\r\n.",
                         context_mode_p->context_ipmf1_mode.compare_mode_2);
        }
        /*
         * Verify that the hashing enabled parameter is legal.
         */
        if (context_mode_p->context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED &&
            context_mode_p->context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported hashing enabled option: %d\r\n.", context_mode_p->context_ipmf1_mode.hash_mode);
        }
        /*
         * Verify that compare and hashing mode do not collide.
         */
        if ((context_mode_p->context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE) &&
            (context_mode_p->context_ipmf1_mode.hash_mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid Field context attributes combinations: compare 2 mode %s can't "
                         "be configured along with hashing = %d\n",
                         dnx_field_context_compare_text(context_mode_p->context_ipmf1_mode.compare_mode_2),
                         context_mode_p->context_ipmf1_mode.hash_mode);
        }
        /*
         * Verify that state table and hashing mode do not collide.
         */
        if ((context_mode_p->state_table_mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED) &&
            (context_mode_p->context_ipmf1_mode.hash_mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED))
        {
            dnx_field_key_id_t hashing_key_id;
            int state_table_use_acr;
            dnx_field_stage_e state_table_field_stage;
            dbal_enum_value_field_field_key_e state_table_single_key_id;
            int state_table_lsb_on_key;
            int state_table_size;
            int key_idx;

            SHR_IF_ERR_EXIT(dnx_field_map_hash_key_id_get(unit, &hashing_key_id));
            SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                            (unit, &state_table_use_acr, &state_table_field_stage, &state_table_single_key_id,
                             &state_table_lsb_on_key, &state_table_size));
            if (state_table_use_acr)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table actions not supported. \n");
            }
            if (state_table_field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                for (key_idx = 0; key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_idx++)
                {
                    if (hashing_key_id.id[key_idx] == state_table_single_key_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Both hashing and state table enabled, and both use "
                                     "key %d on iPMF1.\r\n.", state_table_single_key_id);
                    }
                }
            }
        }
    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Verify that the 'cascade_from' isn't invalid (the same as the range check, only with specific error message.
         */
        cascaded_from = context_mode_p->context_ipmf2_mode.cascaded_from;
        if (cascaded_from == DNX_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The iPMF2 context must have a valid iPMF1 context ID to cascade from.\r\n");
        }
        /*
         * Verify that the context we cascade from is within range.
         */
        if (((unsigned int) cascaded_from) >=
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The IPMF1 context ID to cascade from is %d, exceeds the maximum %d.\r\n",
                         cascaded_from, dnx_data_field.stage.stage_info_get(unit,
                                                                            DNX_FIELD_STAGE_IPMF1)->nof_contexts - 1);
        }
        /*
         * Verifying that we do not try to cascade from IPMF1 to IPMF2 on the same context.
         */
        if ((flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID) && (cascaded_from == *field_context_id_p))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The iPMF1 context that is cascaded from is the same as the iPMF2 context"
                         " to be created (%d).\n"
                         "Whenever an iPMF1 context is created, an iPMF2 context with the same context ID, "
                         "that cascades from the iPMF1 context, is created.\n"
                         "Please create iPMF2 context IDs that cascade from identical context IDs in iPMF1 "
                         "by creating the pair in iPMF1.\r\n", *field_context_id_p);
        }
        /*
         * Verify that the context we cascade from exists.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, cascaded_from, &is_alloc));
        if (!is_alloc)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The IPMF1 context ID to cascade from (%d) does not exist.\r\n", cascaded_from);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_context_compare_resources_reserve(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_mode_info_t * mode_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mode_info_p->mode)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_NONE:
            /*
             * Do Nothing - this is not a COMPARE Context
             */
            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE:
            SHR_IF_ERR_EXIT(dnx_field_key_compare_mode_single_resources_reserve
                            (unit, context_id, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id,
                             &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].key_id));

            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE:
            SHR_IF_ERR_EXIT(dnx_field_key_compare_mode_double_resources_reserve
                            (unit, context_id, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id,
                             &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].key_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal compare mode = %d for context %d, valid values are [%d-%d]\n",
                         mode_info_p->mode, context_id, DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE,
                         DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_context_compare_resources_free(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_mode_info_t * mode_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mode_info_p->mode)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_NONE:
            /*
             * Do Nothing - this is not a COMPARE Context
             */
            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE:
            SHR_IF_ERR_EXIT(dnx_field_key_compare_mode_single_resources_free
                            (unit, context_id, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id,
                             &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].key_id));

            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE:
            SHR_IF_ERR_EXIT(dnx_field_key_compare_mode_double_resources_free
                            (unit, context_id, &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id,
                             &mode_info_p->key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].key_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal compare mode = %d for context %d, valid values are [%d-%d]\n",
                         mode_info_p->mode, context_id, DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE,
                         DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
static shr_error_e
dnx_field_context_resources_reserve(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        dnx_field_context_compare_info_t compare_info;
        dnx_field_context_hashing_info_t hashing_info;
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.get(unit, context_id, &compare_info));

        SHR_IF_ERR_EXIT(dnx_field_context_compare_resources_reserve(unit, stage, context_id, &compare_info.pair_1));
        SHR_IF_ERR_EXIT(dnx_field_context_compare_resources_reserve(unit, stage, context_id, &compare_info.pair_2));

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.get(unit, context_id, &hashing_info));
        if (hashing_info.mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_hash_resources_reserve(unit, context_id, &hashing_info.key_info.key_id));
        }
    }
    if (stage == DNX_FIELD_STAGE_IPMF1 || stage == DNX_FIELD_STAGE_IPMF2)
    {
        dnx_field_context_state_table_sw_info_t state_table_info;

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.get(unit, context_id, &state_table_info));
        if (state_table_info.mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_resources_reserve(unit, stage, context_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
static shr_error_e
dnx_field_context_resources_free(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        dnx_field_context_compare_info_t compare_info;
        dnx_field_context_hashing_info_t hashing_info;
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.get(unit, context_id, &compare_info));

        SHR_IF_ERR_EXIT(dnx_field_context_compare_resources_free(unit, stage, context_id, &compare_info.pair_1));
        SHR_IF_ERR_EXIT(dnx_field_context_compare_resources_free(unit, stage, context_id, &compare_info.pair_2));

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.get(unit, context_id, &hashing_info));
        if (hashing_info.mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_hash_resources_free(unit, context_id, &hashing_info.key_info.key_id));
        }
    }

    if (stage == DNX_FIELD_STAGE_IPMF1 || stage == DNX_FIELD_STAGE_IPMF2)
    {
        dnx_field_context_state_table_sw_info_t state_table_info;

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.get(unit, context_id, &state_table_info));
        if (state_table_info.mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_resources_free(unit, stage, context_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Saves the Context Mode per Context per Stage.
*  Saves also the allocated keys for Compare mode
* \param [in] unit                - Device ID
* \param [in] stage               - Field Stage to create context for
* \param [in] field_context_id    - Context ID
* \param [in] context_mode_p      - context properties struct (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_mode_set(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t field_context_id,
    dnx_field_context_mode_t * context_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        dnx_field_context_ipmf1_sw_info_t context_ipmf1_info;
        dnx_field_context_ipmf2_sw_info_t context_ipmf2_info;

        /*
         * Init the iPMF1/2 SW state info.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf1_sw_info_t_init(unit, &context_ipmf1_info));
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf2_sw_info_t_init(unit, &context_ipmf2_info));

        /*
         * Configure compare mode.
         */
        context_ipmf1_info.compare_info.pair_1.mode = context_mode_p->context_ipmf1_mode.compare_mode_1;
        context_ipmf1_info.compare_info.pair_2.mode = context_mode_p->context_ipmf1_mode.compare_mode_2;
        /** Set the compare to be unset, even though dnx_field_context_compare_info_t_init() should take care of it.*/
        context_ipmf1_info.compare_info.pair_1.is_set = FALSE;
        context_ipmf1_info.compare_info.pair_2.is_set = FALSE;

        if (context_ipmf1_info.compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_compare_key_id_get(unit,
                                                             DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1,
                                                             &context_ipmf1_info.compare_info.
                                                             pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].
                                                             key_id,
                                                             &context_ipmf1_info.compare_info.
                                                             pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].
                                                             key_id));
        }
        if (context_ipmf1_info.compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_compare_key_id_get(unit,
                                                             DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2,
                                                             &context_ipmf1_info.compare_info.
                                                             pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].
                                                             key_id,
                                                             &context_ipmf1_info.compare_info.
                                                             pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].
                                                             key_id));
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Allocated Compare keys: Pair 1 key_id_1 %d key_id_2 %d, Pair 2: key_id_1 %d key_id_2 %d\n",
                     context_ipmf1_info.compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id.id[0],
                     context_ipmf1_info.compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].key_id.id[0],
                     context_ipmf1_info.compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY].key_id.id[0],
                     context_ipmf1_info.compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY].
                     key_id.id[0]);

        /*
         * Configure hashing mode.
         */
        context_ipmf1_info.hashing_info.mode = context_mode_p->context_ipmf1_mode.hash_mode;
        /** Set the hash to be unset, even though dnx_field_context_hashing_info_t_init() should take care of it.*/
        context_ipmf1_info.hashing_info.is_set = FALSE;
        if (context_mode_p->context_ipmf1_mode.hash_mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_hash_key_id_get(unit, &(context_ipmf1_info.hashing_info.key_info.key_id)));
        }

        /*
         * By creating an iPMF1 context, we also create an IPMF2 context with the
         * same context ID that cascades from it. We now update the sw state of the
         * automatically created IPMF2 context.
         */
        context_ipmf2_info.cascaded_from = field_context_id;

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.set(unit, field_context_id, &context_ipmf1_info));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.set(unit, field_context_id, &context_ipmf2_info));
    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        dnx_field_context_ipmf2_sw_info_t context_ipmf2_info;
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf2_sw_info_t_init(unit, &context_ipmf2_info));
        context_ipmf2_info.cascaded_from = context_mode_p->context_ipmf2_mode.cascaded_from;
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.set(unit, field_context_id, &context_ipmf2_info));
    }

    if ((stage == DNX_FIELD_STAGE_IPMF1) || (stage == DNX_FIELD_STAGE_IPMF2))
    {
        /*
         * Configure state table mode. For iPMF1 always, as it includes an iPMF1 context,
         * for iPMF2 only if state table is at iPMF2.
         */
        int configure_state_table = TRUE;
        dnx_field_context_state_table_sw_info_t state_table_info;

        if (stage == DNX_FIELD_STAGE_IPMF2)
        {
            int state_table_use_acr;
            dnx_field_stage_e state_table_field_stage;
            dbal_enum_value_field_field_key_e state_table_single_key_id;
            int state_table_lsb_on_key;
            int state_table_size;

            SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get
                            (unit, &state_table_use_acr, &state_table_field_stage, &state_table_single_key_id,
                             &state_table_lsb_on_key, &state_table_size));
            if (state_table_use_acr)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table actions not supported. \n");
            }
            if (state_table_field_stage != DNX_FIELD_STAGE_IPMF2)
            {
                configure_state_table = FALSE;
            }
        }
        if (configure_state_table)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_t_init(unit, &state_table_info));
            state_table_info.mode = context_mode_p->state_table_mode;
            /*
             * Set the state table to be unset, even though dnx_field_context_state_table_sw_info_t_init()
             * should take care of it.
             */
            state_table_info.is_set = FALSE;
            SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_set(unit, field_context_id, &state_table_info));
        }
    }

    /*
     * All stages except IPMF1 and IPMF2 don't have any SW state to fill for contexts, other then context ID allocation.
     */
exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_mode_t * context_mode_p,
    dnx_field_context_t * field_context_id_p)
{
    int alloc_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_create_verify(unit, flags, stage, context_mode_p, field_context_id_p));

    /** Allocate new context-id*/
    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_allocate(unit, alloc_flags, stage, field_context_id_p));

    /** Set Context Mode */
    SHR_IF_ERR_EXIT(dnx_field_context_mode_set(unit, stage, *field_context_id_p, context_mode_p));

    /**
     * If the there is no given name to the context, we shouldn't set it to SW state.
     * To avoid sw state journal issues.
     */
    if (sal_strncmp(context_mode_p->name, "", sizeof(context_mode_p->name)))
    {
        /** Set the name of the context to the SW state */
        SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                        value.stringncpy(unit, *field_context_id_p, stage, context_mode_p->name));
        /** In case of IPMF1 stage we should set the name of the same IPMF2 context. */
        if (stage == DNX_FIELD_STAGE_IPMF1)
        {
            /** Set the name of the context to the SW state */
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringncpy(unit, *field_context_id_p, DNX_FIELD_STAGE_IPMF2, context_mode_p->name));
        }
    }

    /** Save Resources according to Context Mode */
    SHR_IF_ERR_EXIT(dnx_field_context_resources_reserve(unit, stage, *field_context_id_p));

    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /** Update the FESes and FEMs for a new iPMF2 context to use the FESes of the context it cascades from.*/
        dnx_field_context_t cascaded_from = context_mode_p->context_ipmf2_mode.cascaded_from;
        SHR_IF_ERR_EXIT(dnx_field_group_context_create_fes_update(unit, stage, *field_context_id_p, cascaded_from));
        SHR_IF_ERR_EXIT(dnx_field_efes_action_context_create_fes_update
                        (unit, stage, *field_context_id_p, cascaded_from));
        SHR_IF_ERR_EXIT(dnx_field_group_context_create_fem_update(unit, stage, *field_context_id_p, cascaded_from));
        /** Increase the number of links for the cascaded_from context */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_links_inc(unit, cascaded_from));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_apptype_acl_cs_profile_get(
    int unit,
    dnx_field_context_t acl_context,
    uint32 *cs_profile_id_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, acl_context);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, cs_profile_id_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_apptype_acl_cs_profile_set(
    int unit,
    dnx_field_context_t acl_context,
    uint32 cs_profile_id)
{
    uint32 entry_handle_id;
    uint32 profile_id_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, acl_context);

    /*
     * Sanity check: Verify that there was no profile ID for that context before.
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, &profile_id_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (profile_id_get != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Trying to override profile_id %d with %d for external lookup ACL context %d.\r\n",
                     profile_id_get, cs_profile_id, acl_context);
    }

    /*
     * Write the new profile/
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, cs_profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
static shr_error_e
dnx_field_context_apptype_fwd_cs_profile_set(
    int unit,
    dnx_field_context_t fwd_context,
    uint32 cs_profile_id)
{
    uint32 entry_handle_id;
    uint32 profile_id_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, fwd_context);

    /*
     * Sanity check: Verify that there was no profile ID for that context before.
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_PROFILE, INST_SINGLE, &profile_id_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (profile_id_get != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Trying to override profile_id %d with %d for external lookup FWD context %d.\r\n",
                     profile_id_get, cs_profile_id, fwd_context);
    }

    /*
     * Write the new profile/
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_PROFILE, INST_SINGLE, cs_profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Sets a profile ID to a ceratin FWD context, within iFWD.
*
* \param [in] unit                - Device ID
* \param [in] fwd_context         - fwd_context in iFWD2 to be profiled.
* \param [in] cs_profile_id       - The profile value to write.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_apptype_fwd_cs_profile_ifwd_set(
    int unit,
    dnx_field_context_t fwd_context,
    uint32 cs_profile_id)
{
    uint32 entry_handle_id;
    uint32 profile_id_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD_CONTEXT_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT, fwd_context);

    /*
     * Sanity check: Verify that there was no profile ID for that context before.
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CURRENT_ACL_CONTEXT_PROFILE, INST_SINGLE,
                               &profile_id_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (profile_id_get != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Trying to override profile_id %d with %d for external lookup FWD context %d.\r\n",
                     profile_id_get, cs_profile_id, fwd_context);
    }

    /*
     * Write the new profile/
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_ACL_CONTEXT_PROFILE, INST_SINGLE,
                                 cs_profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_context_apptype_create
*
* \param [in] unit                - Device ID
* \param [in] flags               - Flags for context creation (WITH_ID)
* \param [in] apptype_info_p      - The structure containing the name of the new apptype and the static apptype
*                                   it cascades from.
* \param [in] apptype_p           - Pointer to Apptype, relevant if WITH_ID is set.

* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_apptype_create_verify(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p)
{
    uint32 base_opcode_id;
    uint32 opcode_id;
    uint8 is_alloc;
    int device_locked;
    int external_tcam_exists;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(apptype_info_p, _SHR_E_PARAM, "apptype_info_p");
    SHR_NULL_CHECK(apptype_p, _SHR_E_PARAM, "apptype_p");

    SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &external_tcam_exists));
    if (external_tcam_exists == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring user defined Apptypes (contexts in the external stage) requires an "
                     "external TCAM device.\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_map_is_device_locked(unit, &device_locked));
    if (device_locked)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring user defined Apptypes (contexts in the external stage) cannot "
                     "happen after device lock.\n");
    }

    /*
     * Verify that the source predefined Apptype is legal.
     */
    if ((int) (apptype_info_p->base_static_apptype) < 0 || apptype_info_p->base_static_apptype >= bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When creating a context for for stage external, we are actually creating an apptype. "
                     "The apptype must be based on a predefined apptype with value no more than %d. "
                     "Value for base Apptype is (in cascaded from) %d.\r\n",
                     bcmFieldAppTypeCount - 1, apptype_info_p->base_static_apptype);
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                             (unit, apptype_info_p->base_static_apptype, &base_opcode_id),
                             "When creating a context for for stage external, we are actually creating an apptype. "
                             "The apptype must be based on a predefined apptype."
                             "Value for base Apptype is (in cascaded from) %d.%s%s\r\n",
                             apptype_info_p->base_static_apptype, EMPTY, EMPTY);

    if ((flags & (~DNX_FIELD_CONTEXT_FLAG_WITH_ID)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags (0x%x) includes unknown flags.\r\n", flags);
    }

    /*
     * Verify the Apptype value if given WITH_ID, to provide a clearer error message than resource manager.
     */
    if (flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID)
    {
        if (*apptype_p < dnx_data_field.external_tcam.apptype_user_1st_get(unit) ||
            *apptype_p >=
            dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
            dnx_data_field.external_tcam.apptype_user_nof_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When creating a context for for stage external, we are actually creating an apptype. "
                         "User created apptypes must be within the range %d to %d. Requested Apptype %d with a "
                         "WITH_ID flag.\r\n",
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                         dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1, *apptype_p);
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, *apptype_p, &is_alloc));
        if (is_alloc)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When creating a context for for stage external, we are actually creating an apptype. "
                         "Requested Apptype %d with a WITH_ID flag, but the apptype already exists.\r\n", *apptype_p);
        }
    }

    /** Avoid usage of predefined Apptypes names for user defined Apptypes. */
    for (opcode_id = 0; opcode_id < DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF; opcode_id++)
    {
        uint8 opcode_is_valid;
        uint8 app_type_internal;
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.opcode_predef_info.is_valid.get(unit, opcode_id, &opcode_is_valid));
        if (opcode_is_valid == FALSE)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.opcode_predef_info.apptype.get(unit, opcode_id, &app_type_internal));
        if (sal_strncmp
            (apptype_info_p->name, dnx_field_bcm_apptype_text(unit, app_type_internal),
             sizeof(apptype_info_p->name)) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Requested Apptype name %s is equal to one of the predefined Apptype names %s."
                         "They must be different! \r\n", apptype_info_p->name,
                         dnx_field_bcm_apptype_text(unit, app_type_internal));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_apptype_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p)
{
    int alloc_flags = 0;
    uint32 base_static_opcode_id;
    uint32 opcode_id;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_apptype_create_verify(unit, flags, apptype_info_p, apptype_p));

    /** Allocate new Apptye*/
    SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_allocate(unit, alloc_flags, apptype_p));

    /** Get source static opcode.*/
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                    (unit, apptype_info_p->base_static_apptype, &base_static_opcode_id));

    switch (dnx_data_field.external_tcam.type_get(unit))
    {
        case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
        {
#if defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_create
                            (unit, base_static_opcode_id, apptype_info_p->name, *apptype_p, &opcode_id));
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

    /** Sanity check.*/
    {
        uint8 is_dynamic;
        uint8 opcode_relative_index;
        uint32 opcode_id_get;
        uint8 acl_context;
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, *apptype_p, &opcode_id_get));
        if (opcode_id != opcode_id_get)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "New opcode ID mismatch (%d and %d).\r\n", opcode_id, opcode_id_get);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_is_dynamic(unit, opcode_id, &is_dynamic, &opcode_relative_index));
        if (is_dynamic == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "New opcode %d isn't dynamic.\r\n", opcode_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.acl_context.get(unit, opcode_relative_index, &acl_context));
        if (acl_context == DNX_FIELD_APPTYPE_SWSTATE_ACL_CONTEXT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "New opcode %d (%d) ACL context is invalid (%d).\r\n",
                         opcode_id, opcode_relative_index, acl_context);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_apptype_opcode_cascaded_get(
    int unit,
    uint32 dynamic_opcode,
    uint32 *base_static_opcode_p)
{
    uint8 is_dynamic;
    uint8 opcode_relative_index;
    uint8 base_opcode_uint8;
    uint8 acl_context;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(base_static_opcode_p, _SHR_E_PARAM, "base_static_opcode_p");

    SHR_IF_ERR_EXIT(dnx_field_map_opcode_is_dynamic(unit, dynamic_opcode, &is_dynamic, &opcode_relative_index));
    if (is_dynamic == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d isn't dynamic.\r\n", dynamic_opcode);
    }

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.acl_context.get(unit, opcode_relative_index, &acl_context));
    if (acl_context == DNX_FIELD_APPTYPE_SWSTATE_ACL_CONTEXT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is not allocated.\r\n", dynamic_opcode);
    }

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                    base_opcode.get(unit, opcode_relative_index, &base_opcode_uint8));

    (*base_static_opcode_p) = base_opcode_uint8;

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_apptype_dynamic_opcode_info_get(
    int unit,
    uint32 opcode_id,
    uint32 *base_opcode_id_p,
    char name[DBAL_MAX_STRING_LENGTH])
{
    uint8 is_dynamic;
    uint8 opcode_relative_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_opcode_is_dynamic(unit, opcode_id, &is_dynamic, &opcode_relative_index));
    if (is_dynamic == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d isn't dynamic.\r\n", opcode_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.name.stringget(unit, opcode_relative_index, name));
    SHR_IF_ERR_EXIT(dnx_field_context_apptype_opcode_cascaded_get(unit, opcode_id, base_opcode_id_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_apptype_info_get(
    int unit,
    bcm_field_AppType_t apptype,
    dnx_field_context_apptype_info_t * apptype_info_p)
{
    uint8 is_alloc;
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize the apptype structure. */
    SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, apptype_info_p));

    /**
     * If the given Apptype is dynamic we should return the values, which were set.
     * Otherwise we should return just initial values for predefined Apptypes.
     */
    if (apptype > bcmFieldAppTypeCount)
    {
        uint32 opcode_id;
        uint32 base_opcode;
        if (apptype < dnx_data_field.external_tcam.apptype_user_1st_get(unit) &&
            apptype >= dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
            dnx_data_field.external_tcam.apptype_user_nof_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "User created apptypes must be within the range %d to %d. Value of requested Apptype is %d.\r\n",
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                         dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1, apptype);
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, apptype, &is_alloc));
        if (is_alloc == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Apptype %d is not allocated.\r\n", apptype);
        }

        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_dynamic_opcode_info_get(unit, opcode_id,
                                                                          &base_opcode, apptype_info_p->name));
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, base_opcode,
                                                                   &(apptype_info_p->base_static_apptype)));
    }
    else
    {
        uint32 base_opcode_id;

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                                 (unit, apptype, &base_opcode_id),
                                 "The apptype is within the predefined apptypes range, but is not a known Apptype."
                                 "Value of requested Apptype is %d.%s%s\r\n", apptype, EMPTY, EMPTY);

        sal_strncpy_s(apptype_info_p->name, dnx_field_bcm_apptype_text(unit, apptype), sizeof(apptype_info_p->name));
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_KBP)
/* { */
shr_error_e
dnx_field_context_apptype_opcode_to_child_opcodes(
    int unit,
    uint32 predef_opcode,
    int *nof_child_opcodes_p,
    uint32 user_def_opcodes[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF])
{
    uint8 is_dynamic;
    uint8 predef_index;
    int opcode_index;
    uint8 acl_context;
    uint8 base_opcode;
    uint32 child_opcode;
    int nof_user_defined_apptypes = dnx_data_field.external_tcam.apptype_user_nof_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_child_opcodes_p, _SHR_E_PARAM, "nof_child_opcodes_p");
    SHR_NULL_CHECK(user_def_opcodes, _SHR_E_PARAM, "user_def_opcodes");

    /*
     * Verify the predefined opcode.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_opcode_is_dynamic(unit, predef_opcode, &is_dynamic, &predef_index));
    if (is_dynamic)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "predef_opcode %d must be a predefined opcode.\n", predef_opcode);
    }

    /*
     * Find the child opcodes.
     */
    (*nof_child_opcodes_p) = 0;
    for (opcode_index = 0; opcode_index < nof_user_defined_apptypes; opcode_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.acl_context.get(unit, opcode_index, &acl_context));
        if (acl_context != (typeof(acl_context)) DNX_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.base_opcode.get(unit, opcode_index, &base_opcode));
            if (base_opcode == predef_opcode)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_opcode_dynamic_index_to_opcode(unit, opcode_index, &child_opcode));
                user_def_opcodes[*nof_child_opcodes_p] = child_opcode;
                (*nof_child_opcodes_p)++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* } */
#endif

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_cmp_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_compare_info_t * context_compare_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_compare_info_p, _SHR_E_PARAM, "context_compare_info_p");
    DNX_FIELD_CONTEXT_IS_ALLOCATED(DNX_FIELD_STAGE_IPMF1, context_id, 0);

    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.get(unit, context_id, context_compare_info_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_state_table_sw_info_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_state_table_sw_info_t * state_table_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state_table_info_p, _SHR_E_PARAM, "state_table_info_p");

    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.set(unit, context_id, state_table_info_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_hashing_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_hashing_info_t * context_hashing_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_hashing_info_p, _SHR_E_PARAM, "context_hashing_info_p");
    DNX_FIELD_CONTEXT_IS_ALLOCATED(DNX_FIELD_STAGE_IPMF1, context_id, 0);

    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.get(unit, context_id, context_hashing_info_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_state_table_sw_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_state_table_sw_info_t * state_table_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state_table_info_p, _SHR_E_PARAM, "state_table_info_p");

    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.get(unit, context_id, state_table_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_cascaded_from_context_id_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t * cascaded_from_context_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cascaded_from_context_id_p, _SHR_E_PARAM, "context_cascaded_from_id_p");

    if (stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) cannot have cascaded_from context value. \n",
                     dnx_field_stage_text(unit, stage), stage);
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.context_ipmf2_info.
                    cascaded_from.get(unit, context_id, cascaded_from_context_id_p));

    if (*cascaded_from_context_id_p == DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context %d in stage %s (%d) has invalid cascaded_from value.\n",
                     context_id, dnx_field_stage_text(unit, stage), stage);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_context_next_cascaded_find(
    int unit,
    dnx_field_context_t main_field_context_id,
    unsigned int include_main_field_context_id,
    dnx_field_context_t * cascaded_field_context_id_p)
{
    dnx_field_context_t ipmf2_context_id_ndx, first_ipmf2_context_id_ndx;
    dnx_field_context_t cascaded_field_context_id;
    unsigned int found;

    SHR_FUNC_INIT_VARS(unit);

    cascaded_field_context_id = *cascaded_field_context_id_p;
    if (cascaded_field_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
    {
        /*
         * If input context id is 'invalid' then look for the first.
         */
        first_ipmf2_context_id_ndx = 0;
    }
    else
    {
        if (cascaded_field_context_id >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts)
        {
            /*
             * If input context id is 'out of range' then return 'not found'.
             */
            *cascaded_field_context_id_p = DNX_FIELD_CONTEXT_ID_INVALID;
            SHR_EXIT();
        }
        /*
         * If input context id is 'valid' then look for the 'next'.
         */
        first_ipmf2_context_id_ndx = cascaded_field_context_id + 1;
    }
    found = FALSE;
    for (ipmf2_context_id_ndx = first_ipmf2_context_id_ndx;
         ipmf2_context_id_ndx < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
         ipmf2_context_id_ndx++)
    {
        unsigned int do_search;

        do_search = TRUE;
        if (include_main_field_context_id == FALSE)
        {
            /*
             * Only if the 'include_main_field_context_id' is raised, then do include
             * 'main_field_context_id' itself in the search.
             */
            if (ipmf2_context_id_ndx == main_field_context_id)
            {
                do_search = FALSE;
            }
        }
        if (do_search == TRUE)
        {
            DNX_FIELD_CONTEXT_IS_ALLOCATED(DNX_FIELD_STAGE_IPMF2, ipmf2_context_id_ndx, 1);
            if (!(SHR_FUNC_VAL_IS(_SHR_E_NONE)))
            {
                /*
                 * If returned error was '_SHR_E_NOT_FOUND' then 'ipmf2_context_id_ndx' has not been
                 * allocated for IPMF2. In that case, continue searching.
                 * Otherwise, return with an error.
                 */
                if (!(SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND)))
                {
                    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                    continue;
                }
            }
            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, DNX_FIELD_STAGE_IPMF2, ipmf2_context_id_ndx, &cascaded_field_context_id));
            /*
             * At this point, 'cascaded_field_context_id' contains a valid value.
             */
            if (cascaded_field_context_id == main_field_context_id)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found a context (%d) on IPMF2 which is cascaded from context (%d) on IPMF1 %s%s\r\n",
                             ipmf2_context_id_ndx, main_field_context_id, EMPTY, EMPTY);
                found = TRUE;
                *cascaded_field_context_id_p = ipmf2_context_id_ndx;
                break;
            }
        }
    }
    if (found != TRUE)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "\r\n"
                     "Could not find a context (%d) on IPMF2 which is cascaded from context (%d) on IPMF1 starting from context %d %s\r\n",
                     ipmf2_context_id_ndx, main_field_context_id, first_ipmf2_context_id_ndx, EMPTY);
        *cascaded_field_context_id_p = DNX_FIELD_CONTEXT_ID_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify function for dnx_field_context_destroy()
 * \param [in] unit              - Device id
 * \param [in] stage             - field stage
 * \param [in] field_context_id  - Context id to destroy
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_context_destroy_verify(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t field_context_id)
{
    unsigned int attached_context_id_arr_ndx;
    dnx_field_context_t ipmf2_context_id_ndx;
    uint8 is_alloc;
    dnx_field_context_t cascaded_from;
    unsigned int fg_id_ndx;
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    uint8 is_set;
    dnx_field_stage_e fg_stage;
    dnx_field_presel_t presel_id_ndx;
    unsigned int nof_presels;
    dnx_field_presel_entry_id_t entry_id;
    dnx_field_presel_entry_data_t entry_data;
    int nof_fgs = dnx_data_field.group.nof_fgs_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, field_context_id, 0);

    /*
     * Verify that the stage is a PMF stage.
     */
    if (stage == DNX_FIELD_STAGE_ACE || stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage not a PMF stage, received stage \"%s\".\r\n",
                     dnx_field_stage_text(unit, stage));
    }

    /*
     * Verify that the context to destroy isn't the default context.
     */
    if (field_context_id == DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy context ID %d, since it is the default context.\r\n",
                     field_context_id);
    }

    /*
     * If we destroy an iPMF1 context, check that it doesn't have any non default context IDs cascading
     * from it in iPMF2.
     * If we destroy an iPMF2 context, check that it doesn't inherit the same context ID as its own,
     * then is should be destroyed by destroying the iPMF1 context.
     */
    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        for (ipmf2_context_id_ndx = 0;
             ipmf2_context_id_ndx < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
             ipmf2_context_id_ndx++)
        {
            if (ipmf2_context_id_ndx != field_context_id)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                                (unit, DNX_FIELD_STAGE_IPMF2, ipmf2_context_id_ndx, &is_alloc));
                if (is_alloc)
                {
                    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.
                                    cascaded_from.get(unit, ipmf2_context_id_ndx, &cascaded_from));
                    if (cascaded_from == field_context_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy context ID %d in iPMF1, it cascades to "
                                     "context ID %d in iPMF2.\r\n", field_context_id, ipmf2_context_id_ndx);
                    }
                }
            }
        }
    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Note we check that the context to be destroyed is allocated even though the deallocation should take
         * care of it, because otherwise we cannot safely access its SW state (and expect valid information).
         * However, for conformity of error messages, we will not send en error message if it isn't allocated,
         * we'll let it pass the verification and fail during deallocation.
         * Also note that if the 'cascaded_from' information is in accordance with the allocation status,
         * that is if the iPMF2 context is the default cascading of the same context ID in IPMF1 and the context ID
         * is allocated in IPMF1, even if we didn't send an error message here the deallocation algorithm will send
         * an error.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF2, field_context_id, &is_alloc));
        if (is_alloc)
        {
            SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.cascaded_from.get(unit, field_context_id, &cascaded_from));
            if (cascaded_from == field_context_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Context ID %d in iPMF2 is the default cascading for the same context ID in iPMF1. "
                             "Please delete it by deleting the iPMF1 context.\r\n", field_context_id);
            }
        }
    }

    /*
     * Verify that no field group is attached to the context.
     */
    for (fg_id_ndx = 0; fg_id_ndx < nof_fgs; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
        if (is_alloc)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
            if (fg_stage == stage)
            {
                SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_ndx, context_id_arr));
                for (attached_context_id_arr_ndx = 0;
                     attached_context_id_arr_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS
                     && context_id_arr[attached_context_id_arr_ndx] != DNX_FIELD_CONTEXT_ID_INVALID;
                     attached_context_id_arr_ndx++)
                {
                    if (context_id_arr[attached_context_id_arr_ndx] == field_context_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Field group %d is attached to context ID %d in stage %s (%d).\r\n",
                                     fg_id_ndx, field_context_id, dnx_field_stage_text(unit, stage), stage);
                    }
                }
            }
        }
    }

    /*
     * If we destroy an iPMF1 context, verify that it doesn't have compare or hash set.
     * Note we do not check the mode, if compare or hash were enabled to begin with.
     */
    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.pair_1.is_set.get(unit, field_context_id, &is_set));
        if (is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Compare pair 1 is set, please call compare destroy first.\r\n");
        }
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.pair_2.is_set.get(unit, field_context_id, &is_set));
        if (is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Compare pair 2 is set, please call compare destroy first.\r\n");
        }
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.is_set.get(unit, field_context_id, &is_set));
        if (is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Hashing is set, please call hash destroy first.\r\n");
        }
    }
    /*
     * Verify that no preselector is attached to the context.
     */
    nof_presels = dnx_data_field.stage.stage_info_get(unit, stage)->nof_cs_lines;
    if (nof_presels <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported stage: %s.\n", dnx_field_stage_text(unit, stage));
    }
    for (presel_id_ndx = 0; presel_id_ndx < nof_presels; presel_id_ndx++)
    {
        entry_id.presel_id = presel_id_ndx;
        entry_id.stage = stage;
        SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, 0, &entry_id, &entry_data));
        if (entry_data.entry_valid && entry_data.context_id == field_context_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Context %d in stage %s is being pointed to by presel Id %d.\n",
                         field_context_id, dnx_field_stage_text(unit, stage), presel_id_ndx);
        }
    }

    /*
     * If the context is in IPMF1 we also need to check that no presel points to the default cascading context in iPMF2.
     * Note we have already checked that not other contexts cascade from the iPMF1 context.
     */
    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        for (presel_id_ndx = 0;
             presel_id_ndx < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_cs_lines;
             presel_id_ndx++)
        {
            entry_id.presel_id = presel_id_ndx;
            entry_id.stage = DNX_FIELD_STAGE_IPMF2;
            SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, 0, &entry_id, &entry_data));
            if (entry_data.entry_valid && entry_data.context_id == field_context_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Context %d in stage %s (default cascaded context) is "
                             "being pointed to by presel Id %d.\n",
                             field_context_id, dnx_field_stage_text(unit, DNX_FIELD_STAGE_IPMF2), presel_id_ndx);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h*/
shr_error_e
dnx_field_context_destroy(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t field_context_id)
{
    dnx_field_context_ipmf1_sw_info_t context_ipmf1_info;
    dnx_field_context_ipmf2_sw_info_t context_ipmf2_info;
    dnx_field_context_state_table_sw_info_t state_table_info;
    dnx_field_context_t cascaded_from;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_destroy_verify(unit, stage, field_context_id));

    /** Delete the FESes for an iPMF2 context that come from the context it cascades from.*/
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get(unit, stage, field_context_id, &cascaded_from));
        SHR_IF_ERR_EXIT(dnx_field_group_context_destroy_fes_update(unit, stage, field_context_id, cascaded_from));
        SHR_IF_ERR_EXIT(dnx_field_group_context_destroy_fem_update(unit, stage, field_context_id, cascaded_from));
        /** Decrease the number of links for the cascaded_from context */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_links_dec(unit, cascaded_from));
    }

    SHR_IF_ERR_EXIT(dnx_field_context_resources_free(unit, stage, field_context_id));

    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_deallocate(unit, stage, field_context_id));

    /*
     * Delete the SW state.
     */
    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf1_sw_info_t_init(unit, &context_ipmf1_info));
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf2_sw_info_t_init(unit, &context_ipmf2_info));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.set(unit, field_context_id, &context_ipmf1_info));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.set(unit, field_context_id, &context_ipmf2_info));

    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * For iPMF2 we delete only the SW state of iPMF2.
         * Technically, we could have deleted the SW state for both stages.
         */
        SHR_IF_ERR_EXIT(dnx_field_context_ipmf2_sw_info_t_init(unit, &context_ipmf2_info));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.set(unit, field_context_id, &context_ipmf2_info));
    }
    if (stage == DNX_FIELD_STAGE_IPMF1 || stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_t_init(unit, &state_table_info));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.set(unit, field_context_id, &state_table_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_context.h
 */
shr_error_e
dnx_field_context_mode_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t * context_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_mode_p, _SHR_E_PARAM, "context_mode_p");

    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 1);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, context_mode_p));

    /** Get the ode of the context and return it in the dnx_field_context_mode_t structure. */
    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                    value.stringget(unit, context_id, stage, context_mode_p->name));

    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.pair_1.
                        mode.get(unit, context_id, &context_mode_p->context_ipmf1_mode.compare_mode_1));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.pair_2.
                        mode.get(unit, context_id, &context_mode_p->context_ipmf1_mode.compare_mode_2));
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.
                        mode.get(unit, context_id, &context_mode_p->context_ipmf1_mode.hash_mode));
    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF2_INFO.
                        cascaded_from.get(unit, context_id, &context_mode_p->context_ipmf2_mode.cascaded_from));
    }

    if (stage == DNX_FIELD_STAGE_IPMF1 || stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_STATE_TABLE_INFO.
                        mode.get(unit, context_id, &context_mode_p->state_table_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_compare_pair_info_t_init(
    int unit,
    dnx_field_key_compare_pair_info_t * pair_info_p)
{

    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pair_info_p, _SHR_E_PARAM, "pair_info_p");

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        pair_info_p->first_key_info.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &pair_info_p->first_key_info.qual_info[ii]));
        pair_info_p->second_key_info.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &pair_info_p->second_key_info.qual_info[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_hash_info_t_init(
    int unit,
    dnx_field_context_hash_info_t * hash_info_p)
{

    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(hash_info_p, _SHR_E_PARAM, "hash_info_p");

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        hash_info_p->key_info.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &hash_info_p->key_info.qual_info[ii]));
    }
    /** Initializes the comparison and order variables to disabled */
    hash_info_p->compression = FALSE;
    hash_info_p->order = FALSE;

    hash_info_p->hash_function = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_INVALID;

    /** Initializes the hash config data */
    for (ii = 0; ii < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS; ii++)
    {
        hash_info_p->hash_config[ii].action_key = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_INVALID;
        hash_info_p->hash_config[ii].crc_select = DBAL_ENUM_FVAL_CRC_SELECT_INVALID;
        hash_info_p->hash_config[ii].hash_action = DBAL_ENUM_FVAL_HASH_ACTION_NONE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_state_table_info_t_init(
    int unit,
    dnx_field_context_state_table_info_t * state_table_info_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state_table_info_p, _SHR_E_PARAM, "state_table_info_p");

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        state_table_info_p->address.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        state_table_info_p->data.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        state_table_info_p->opcode_based_qual.dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &state_table_info_p->address.qual_info[ii]));
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &state_table_info_p->data.qual_info[ii]));
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &state_table_info_p->opcode_based_qual.qual_info[ii]));
    }
    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        state_table_info_p->current_state.dnx_actions[ii] = DNX_FIELD_ACTION_TYPE_INVALID;
        state_table_info_p->next_state.dnx_actions[ii] = DNX_FIELD_ACTION_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_action_attach_info_t_init(unit, state_table_info_p->current_state.action_info));
        SHR_IF_ERR_EXIT(dnx_field_action_attach_info_t_init(unit, state_table_info_p->next_state.action_info));
    }
    state_table_info_p->opcode = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_QUAL;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Sets the given key for a context feature
* \param [in] unit           - Device ID
* \param [in] flags          - TBD
* \param [in] stage          - Stage of the context
* \param [in] context_id     - The context to configure the compare key for
* \param [in] feature        - The feature to set the key for
* \param [in] expected_key_size - Size of the key
* \param [in] qual_info_p    - The key to set quals
* \param [in,out] key_info_p - Holds key id as input, and returns key template
*                              as output
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_feature_key_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_feature_e feature,
    int expected_key_size,
    dnx_field_context_key_qual_info_t * qual_info_p,
    dnx_field_context_key_info_t * key_info_p)
{
    dnx_field_key_attach_info_in_t key_in_info;
    dbal_enum_value_field_pmf_lookup_type_e pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    /** Init Key In info */
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));

    sal_memcpy(key_in_info.qual_info, qual_info_p->qual_info, sizeof(key_in_info.qual_info));

    key_in_info.field_stage = stage;

    /** Create Key template */
    switch (feature)
    {
        case DNX_FIELD_CONTEXT_FEATURE_COMPARE:
        {
            SHR_SET_CURRENT_ERR(dnx_field_key_template_create
                                (unit, key_in_info.field_stage, qual_info_p->dnx_quals, &key_in_info.key_template));
            if (key_in_info.key_template.key_size_in_bits > expected_key_size)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Given compare key size (%d) is larger than maximum allowed (%d). "
                             "Conext %d, stage \"%s\".\n",
                             key_in_info.key_template.key_size_in_bits, expected_key_size,
                             context_id, dnx_field_stage_text(unit, stage));
            }
            break;
        }
        case DNX_FIELD_CONTEXT_FEATURE_HASHING:
        {
            SHR_SET_CURRENT_ERR(dnx_field_key_template_create
                                (unit, key_in_info.field_stage, qual_info_p->dnx_quals, &key_in_info.key_template));
            if (key_in_info.key_template.key_size_in_bits > expected_key_size)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Given hash key size (%d) is larger than maximum allowed (%d). "
                             "Conext %d, stage \"%s\".\n",
                             key_in_info.key_template.key_size_in_bits, expected_key_size,
                             context_id, dnx_field_stage_text(unit, stage));
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Feature %d not supported", feature);
        }
    }

    /** Attach the key for the given context */
    SHR_IF_ERR_EXIT(dnx_field_key_context_feature_key_attach(unit, context_id, &key_in_info, &key_info_p->key_id));

    /** Changes the state of the given lookup interface type */
    SHR_IF_ERR_EXIT(dnx_field_key_lookup_set(unit, stage, context_id, pmf_lookup_type, TRUE));

    /** Update info with new key template/id */
    sal_memcpy(&(key_info_p->key_template), &(key_in_info.key_template), sizeof(key_info_p->key_template));

    /** Update attach info. */
    sal_memcpy(&(key_info_p->attach_info), qual_info_p->qual_info, sizeof(key_info_p->attach_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_context_feature_key_unset(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * key_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_context_feature_key_detach
                    (unit, stage, context_id, &key_info_p->key_id, &key_info_p->key_template));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_context_hash_create().
* \param [in] unit         - Device ID
* \param [in] flags        - Flags (future use)
* \param [in] stage        - Field Stage
* \param [in] context_id   - Field Context ID
* \param [in] hash_info_p  - structure for hash key info (see filed_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_hash_create_verify(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p)
{
    dnx_field_context_hashing_info_t hashing_info;
    dnx_field_context_hash_info_t hash_get_info;
    dnx_field_context_mode_t context_mode;
    int qual_iter;
    int hash_config_iter, hash_config_iter2;
    uint8 cascaded;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(hash_info_p, _SHR_E_PARAM, "hash_info_p");

    if (stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Hash' can only be configured on iPMF1 contexts. "
                     "Context %d , stage %s are not valid for hashing \r\n",
                     context_id, dnx_field_stage_text(unit, stage));
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, stage, context_id, &context_mode));

    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));
    if (hashing_info.mode != DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hashing mode is not set to enabled for context %d .\r\n", context_id);
    }

    if (context_mode.context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context %d cannot have Compare mode Double and Hashing Enabled."
                     "There will be an overlap in key usage\r\n", context_id);
    }

    if (flags & DNX_FIELD_CONTEXT_HASH_FLAGS_UPDATE)
    {
        if (!hashing_info.is_set)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "'Hash' was not set. Please call hash create before update.\r\n");
        }
        /** key_info cannot be changed on UPDATE mode */
        for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             hash_info_p->key_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID &&
             hashing_info.key_info.key_template.key_qual_map[qual_iter].qual_type != DNX_FIELD_QUAL_TYPE_INVALID;
             qual_iter++)
        {
            if (hash_info_p->key_info.dnx_quals[qual_iter] !=
                hashing_info.key_info.key_template.key_qual_map[qual_iter].qual_type
                || hash_info_p->key_info.qual_info[qual_iter].input_type !=
                hashing_info.key_info.attach_info[qual_iter].input_type
                || hash_info_p->key_info.qual_info[qual_iter].input_arg !=
                hashing_info.key_info.attach_info[qual_iter].input_arg
                || hash_info_p->key_info.qual_info[qual_iter].offset !=
                hashing_info.key_info.attach_info[qual_iter].offset)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier %s at index %d was changed. key info for context %d cannot be changed on UPDATE mode.\r\n",
                             dnx_field_dnx_qual_text(unit, hash_info_p->key_info.dnx_quals[qual_iter]), qual_iter,
                             context_id);
            }
        }
        if (qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
        {
            if (hash_info_p->key_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier %s at index %d was added. key info for context %d cannot be changed on UPDATE mode.\r\n",
                             dnx_field_dnx_qual_text(unit, hash_info_p->key_info.dnx_quals[qual_iter]), qual_iter,
                             context_id);
            }
            if (hashing_info.key_info.key_template.key_qual_map[qual_iter].qual_type != DNX_FIELD_QUAL_TYPE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier %s was removed. key info for context %d cannot be changed on UPDATE mode.\r\n",
                             dnx_field_dnx_qual_text(unit,
                                                     hashing_info.key_info.key_template.
                                                     key_qual_map[qual_iter].qual_type), context_id);
            }
        }
        /** currently compression and order cannot be changed on UPDATE mode, because it cannot be done together with hash_config and hash_function in a traffic safe way */
        SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &hash_get_info));
        SHR_IF_ERR_EXIT(dnx_field_context_hash_info_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &hash_get_info));
        if (hash_info_p->compression != hash_get_info.compression)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "compression for context %d cannot be changed on UPDATE mode.\r\n", context_id);
        }
        if (hash_info_p->order != hash_get_info.order)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "order for context %d cannot be changed on UPDATE mode.\r\n", context_id);
        }
    }
    else
    {
        if (hashing_info.is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS,
                         "'Hash' was already set. Please call hash destroy before create or call create with UPDATE flag.\r\n");
        }
        /** Verify that each configured action_key, has a not-none hash_action */
        for (hash_config_iter = 0;
             hash_config_iter < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS
             && hash_info_p->hash_config[hash_config_iter].action_key != DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_INVALID;
             hash_config_iter++)
        {
            if (hash_info_p->hash_config[hash_config_iter].hash_action == DBAL_ENUM_FVAL_HASH_ACTION_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Hash action wasn't configured for action key 0x%x. must be configured when creating context hash.\n",
                             hash_info_p->hash_config[hash_config_iter].action_key);
            }
        }
    }

    /** Verify hash_function is defined */
    if (hash_info_p->hash_function == DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hash function for context %d must be defined. \r\n", context_id);
    }
    /** Verify that no key_action nor crc_select appear twice */
    for (hash_config_iter = 0;
         hash_config_iter < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS
         && hash_info_p->hash_config[hash_config_iter].action_key != DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_INVALID;
         hash_config_iter++)
    {
        for (hash_config_iter2 = 0; hash_config_iter2 < hash_config_iter; hash_config_iter2++)
        {
            if (hash_info_p->hash_config[hash_config_iter].action_key ==
                hash_info_p->hash_config[hash_config_iter2].action_key)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Action key 0x%x appears twice.\n",
                             hash_info_p->hash_config[hash_config_iter].action_key);
            }
            if (hash_info_p->hash_config[hash_config_iter].crc_select ==
                hash_info_p->hash_config[hash_config_iter2].crc_select
                && hash_info_p->hash_config[hash_config_iter].crc_select != DBAL_ENUM_FVAL_CRC_SELECT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "CRC select 0x%x appears twice.\n",
                             hash_info_p->hash_config[hash_config_iter].crc_select);
            }
        }
    }

    /** Verify that crc_select is configured only for augmentation action */
    for (hash_config_iter = 0;
         hash_config_iter < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS
         && hash_info_p->hash_config[hash_config_iter].action_key != DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_INVALID;
         hash_config_iter++)
    {
        if (hash_info_p->hash_config[hash_config_iter].crc_select == DBAL_ENUM_FVAL_CRC_SELECT_INVALID &&
            (hash_info_p->hash_config[hash_config_iter].hash_action == DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_CRC
             || hash_info_p->hash_config[hash_config_iter].hash_action == DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_KEY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "CRC Select must be specified when selecting an augment function.\n");
        }

        if (hash_info_p->hash_config[hash_config_iter].crc_select != DBAL_ENUM_FVAL_CRC_SELECT_INVALID &&
            (hash_info_p->hash_config[hash_config_iter].hash_action == DBAL_ENUM_FVAL_HASH_ACTION_NONE
             || hash_info_p->hash_config[hash_config_iter].hash_action == DBAL_ENUM_FVAL_HASH_ACTION_REPLACE_CRC))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " CRC Select must not be specified when not selecting an augment function\n");
        }
    }
    DNX_FIELD_IS_ANY_QUAL_CASCADED(unit, hash_info_p->key_info.qual_info, cascaded);
    if (cascaded == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Hashing context(%d) cannot have qualifier with Cascaded input type. \r\n",
                     context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Configure compression and order for hash context
* \param [in] unit       - Device ID
* \param [in] context_id - Field Context ID
* \param [in] hash_info_p  - structure for hash key info (see filed_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_hash_compression_and_order(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPRESSION, INST_SINGLE, hash_info_p->compression);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWITCH, INST_SINGLE, hash_info_p->order);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Configure CRC select for hash context
* \param [in] unit       - Device ID
* \param [in] flags      - flags (can be UPDATE)
* \param [in] context_id - Field Context ID
* \param [in] hash_info_p  - structure for hash key info (see field_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_hash_configuration(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p)
{
    uint32 entry_handle_id;
    uint8 ii;
    dbal_enum_value_field_field_hash_lb_key_e action_key_iter;
    dbal_enum_value_field_crc_select_e free_crc_select = 0;
    int *crc_selects_used = NULL;
    int nof_crc_selects_used;
    int nof_action_keys_used;
    uint32 enum_iterator;
    dbal_enum_value_field_crc_select_e *crc_selects = NULL;
    dbal_enum_value_field_hash_action_e *hash_actions = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Count the number of possible crc selects.
     * Allocating crc_selects_used.
     */
    nof_crc_selects_used = 0;
    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_CRC_SELECT, INVALID_ENUM, &enum_iterator));
    while (enum_iterator != INVALID_ENUM)
    {
        nof_crc_selects_used++;
        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_CRC_SELECT, enum_iterator, &enum_iterator));
    }
    SHR_ALLOC_SET_ZERO(crc_selects_used, nof_crc_selects_used * sizeof(crc_selects_used[0]), "crc_selects_used",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    /*
     * Count the number of possible action keys.
     * Allocating crc_selects and hash_actions.
     */
    nof_action_keys_used = 0;
    SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                    (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, INVALID_ENUM, &enum_iterator));
    while (enum_iterator != INVALID_ENUM)
    {
        nof_action_keys_used++;
        SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                        (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, enum_iterator, &enum_iterator));
    }
    SHR_ALLOC_SET_ZERO(crc_selects, nof_action_keys_used * sizeof(crc_selects[0]), "crc_selects",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(hash_actions, nof_action_keys_used * sizeof(hash_actions[0]), "hash_actions",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** initialize hash_config */
    for (action_key_iter = 0; action_key_iter < nof_action_keys_used; action_key_iter++)
    {
        crc_selects[action_key_iter] = DBAL_ENUM_FVAL_CRC_SELECT_INVALID;
        hash_actions[action_key_iter] = DBAL_ENUM_FVAL_HASH_ACTION_NONE;
    }

    /** get the current state and unused crc_selects */
    for (ii = 0;
         ii < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS
         && hash_info_p->hash_config[ii].action_key != DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_INVALID; ii++)
    {
        hash_actions[hash_info_p->hash_config[ii].action_key] = hash_info_p->hash_config[ii].hash_action;
        if (hash_info_p->hash_config[ii].crc_select != DBAL_ENUM_FVAL_CRC_SELECT_INVALID)
        {
            crc_selects[hash_info_p->hash_config[ii].action_key] = hash_info_p->hash_config[ii].crc_select;
            crc_selects_used[hash_info_p->hash_config[ii].crc_select] = 1;
        }
    }

    /** make the commit */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION_WITH_KEY, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HASH_FUNC, INST_SINGLE, hash_info_p->hash_function);

    for (action_key_iter = 0; action_key_iter < nof_action_keys_used; action_key_iter++)
    {
        if (crc_selects[action_key_iter] == DBAL_ENUM_FVAL_CRC_SELECT_INVALID)
        {
            /*
             * get a free crc_select for the unused action_keys and non-augment actions 
             */
            for (; free_crc_select < nof_crc_selects_used && crc_selects_used[free_crc_select]; free_crc_select++);
            crc_selects[action_key_iter] = free_crc_select;
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_SELECT, action_key_iter,
                                     crc_selects[action_key_iter]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HASH_ACTION, action_key_iter,
                                     hash_actions[action_key_iter]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(crc_selects_used);
    SHR_FREE(crc_selects);
    SHR_FREE(hash_actions);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_hash_create(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p)
{
    dnx_field_context_hashing_info_t hashing_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_hash_create_verify(unit, flags, stage, context_id, hash_info_p));

    if (!(flags & DNX_FIELD_CONTEXT_HASH_FLAGS_UPDATE))
    {
        /** Make DBAL commit to DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION */
        SHR_IF_ERR_EXIT(dnx_field_context_hash_compression_and_order(unit, context_id, hash_info_p));
    }

    /** Make DBAL commit to DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION_WITH_KEY */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_configuration(unit, flags, context_id, hash_info_p));

    if (!(flags & DNX_FIELD_CONTEXT_HASH_FLAGS_UPDATE))
    {
        int key_size = dnx_data_field.hash.max_key_size_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));

        SHR_IF_ERR_EXIT(dnx_field_context_feature_key_set
                        (unit, flags, stage, context_id, DNX_FIELD_CONTEXT_FEATURE_HASHING, key_size,
                         &hash_info_p->key_info, &hashing_info.key_info));
        hashing_info.is_set = TRUE;

        SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.set(unit, context_id, &hashing_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Function to set the retrieved attach info from dnx_field_context_key_info_t
*  to dnx_field_context_key_qual_info_t.
*  Used in dnx_field_context_hash_info_get() and dnx_field_context_compare_info_get().
* \param [in] unit         - Device ID
* \param [in] key_info_p   - Field Context ID
* \param [out] key_qual_info_p  - structure for hash key info (see filed_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static void
dnx_field_context_hash_cmp_attach_info_set(
    int unit,
    dnx_field_context_key_info_t * key_info_p,
    dnx_field_context_key_qual_info_t * key_qual_info_p)
{
    unsigned int qual_index;

    for (qual_index = 0; (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
         && (key_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_index++)
    {
        key_qual_info_p->dnx_quals[qual_index] = key_info_p->key_template.key_qual_map[qual_index].qual_type;
    }
    sal_memcpy(key_qual_info_p->qual_info, key_info_p->attach_info, sizeof(key_qual_info_p->qual_info));
}

/** See field_context.h */
shr_error_e
dnx_field_context_hash_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p)
{
    dnx_field_context_hashing_info_t hashing_info;
    uint32 entry_handle_id;
    uint8 action_key_iter;
    int hash_config_iter;
    uint32 *crc_selects = NULL;
    uint32 *hash_actions = NULL;
    int nof_action_keys_used;
    uint32 enum_iterator;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Count the number of possible action keys.
     * Allocating crc_selects and hash_actions.
     */
    nof_action_keys_used = 0;
    SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                    (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, INVALID_ENUM, &enum_iterator));
    while (enum_iterator != INVALID_ENUM)
    {
        nof_action_keys_used++;
        SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                        (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, enum_iterator, &enum_iterator));
    }
    SHR_ALLOC_SET_ZERO(crc_selects, nof_action_keys_used * sizeof(crc_selects[0]), "crc_selects",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(hash_actions, nof_action_keys_used * sizeof(hash_actions[0]), "hash_actions",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));

    dnx_field_context_hash_cmp_attach_info_set(unit, &hashing_info.key_info, &hash_info_p->key_info);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_COMPRESSION, INST_SINGLE, &hash_info_p->compression);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SWITCH, INST_SINGLE, &hash_info_p->order);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION_WITH_KEY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HASH_FUNC, INST_SINGLE, &hash_info_p->hash_function);

    for (action_key_iter = 0; action_key_iter < nof_action_keys_used; action_key_iter++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRC_SELECT, action_key_iter,
                                   &(crc_selects[action_key_iter]));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HASH_ACTION, action_key_iter,
                                   &(hash_actions[action_key_iter]));
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * find all action keys that were configured with hash action
     */
    hash_config_iter = 0;
    for (action_key_iter = 0; action_key_iter < nof_action_keys_used; action_key_iter++)
    {
        if (hash_actions[action_key_iter] != DBAL_ENUM_FVAL_HASH_ACTION_NONE)
        {
            hash_info_p->hash_config[hash_config_iter].action_key = action_key_iter;
            hash_info_p->hash_config[hash_config_iter].crc_select = crc_selects[action_key_iter];
            hash_info_p->hash_config[hash_config_iter++].hash_action = hash_actions[action_key_iter];
        }
    }

exit:
    SHR_FREE(crc_selects);
    SHR_FREE(hash_actions);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_context_hash_destroy().
* \param [in] unit       - Device ID
* \param [in] flags      - flags (future use)
* \param [in] stage      - Field Stage
* \param [in] context_id - Field Context ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_context_hash_destroy_verify(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    dnx_field_context_hashing_info_t hashing_info;
    SHR_FUNC_INIT_VARS(unit);

    if (stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Hash' can only be configured on iPMF1.\r\n");
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));
    if (hashing_info.mode != DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hashing mode is not set to enabled.\r\n");
    }

    if (hashing_info.is_set == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "'Hash' was not set.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_hash_destroy(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    dnx_field_context_hashing_info_t hashing_info;
    uint32 entry_handle_id;
    uint32 action_key_iter;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_hash_destroy_verify(unit, flags, stage, context_id));

    /** To maintain correctness of crc_select mechanism, we want keep the values of this field,
     *  to make sure all the action keys will be configured to different CRC select at all times,
     *  so we'll clear only the hash_action field */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION_WITH_KEY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HASH_FUNC, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                    (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, INVALID_ENUM, &action_key_iter));
    while (action_key_iter != INVALID_ENUM)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HASH_ACTION, action_key_iter,
                                     DBAL_ENUM_FVAL_HASH_ACTION_NONE);
        SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                        (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, action_key_iter, &action_key_iter));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** Retrieve all hashing info from SW-state */
    SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));

    /** Detach the hash key for this context */
    SHR_IF_ERR_EXIT(dnx_field_context_feature_key_unset(unit, flags, stage, context_id, &hashing_info.key_info));

    /** Hashing info is no longer set */
    hashing_info.is_set = FALSE;

    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.hashing_info.set(unit, context_id, &hashing_info));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_context_state_table_attach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    dnx_field_context_state_table_sw_info_t state_table_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_get(unit, context_id, &state_table_info));

    if (state_table_info.mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "State Table is disabled on context %d", context_id);
    }
    if (state_table_info.is_set)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "State Table is already configured on context %d, please remove existing State "
                     "Table field group first", context_id);
    }

    state_table_info.is_set = TRUE;
    SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_set(unit, context_id, &state_table_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_context_state_table_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    dnx_field_context_state_table_sw_info_t state_table_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_get(unit, context_id, &state_table_info));

    state_table_info.is_set = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_context_state_table_sw_info_set(unit, context_id, &state_table_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_context_compare_set()
* \param [in] unit          - Device ID
* \param [in] flags         - TBD
* \param [in] stage         - Field Stage
* \param [in] context_id    - Field Context ID
* \param [in] pair_id       - Identifier of the pair ID(Can be either 1 or 2).
* \param [in] cmp_pair_p    - The first pair of keys information. Each key information is composed of
*                             DNX quals(types/info).
*/
static shr_error_e
dnx_field_context_compare_create_verify(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * cmp_pair_p)
{
    dnx_field_context_compare_info_t compare_info;
    dnx_field_context_mode_t context_mode;
    uint8 cascaded;
    SHR_FUNC_INIT_VARS(unit);

    if (stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Compare' can only be configured on iPMF1.\r\n");
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(stage, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, stage, context_id, &context_mode));

    if (context_mode.context_ipmf1_mode.compare_mode_1 == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE &&
        context_mode.context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context %d both compare modes are disabled", context_id);
    }

    if (pair_id != DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR && pair_id != DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pair ID %d is not a valid. Legal values are %d and %d \n", pair_id,
                     DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR, DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR);
    }

    /*
     * Verify that info is provided for at least one pair.
     */
    if (cmp_pair_p == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'cmp_pair_p' is NULL. Info must be provided for at least one compare pair.\r\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));

    /*
     * Verify first pair
     */
    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        /*
         * Verify that compare is enabled for the first pair.
         */
        if (compare_info.pair_1.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Compare is not enabled for the first pair in context ID %d stage %s (%d).\r\n",
                         context_id, dnx_field_stage_text(unit, stage), stage);
        }
        else if (compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE
                 && compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown compare mode for first pair (%d).\r\n", compare_info.pair_1.mode);
        }
        if ((compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE) &&
            (cmp_pair_p->second_key_info.dnx_quals[0] != DNX_FIELD_QUAL_TYPE_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Compare for the first pair in context ID %d stage %s needs "
                         "to be mode double to take two keys.\r\n", context_id, dnx_field_stage_text(unit, stage));
        }
        /*
         * Verify that the pair hasn't already been set.
         */
        if (compare_info.pair_1.is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Compare was already set for the first pair. "
                         "Please call compare destroy first.\r\n");
        }

    }

    /*
     * Verify second pair
     */
    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        /*
         * Verify that compare is enabled for the second pair.
         */
        if (compare_info.pair_2.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Compare is not enabled for the second pair in context ID %d stage %s (%d).\r\n",
                         context_id, dnx_field_stage_text(unit, stage), stage);
        }
        else if (compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE
                 && compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown compare mode for second pair (%d).\r\n", compare_info.pair_2.mode);
        }
        if ((compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE) &&
            (cmp_pair_p->second_key_info.dnx_quals[0] != DNX_FIELD_QUAL_TYPE_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Compare for the second pair in context ID %d stage %s needs "
                         "to be mode double to take two keys.\r\n", context_id, dnx_field_stage_text(unit, stage));
        }
        /*
         * Verify that the pair hasn't already been set.
         */
        if (compare_info.pair_2.is_set)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Compare was already set for the second pair. "
                         "Please call compare destroy first.\r\n");
        }
    }
    DNX_FIELD_IS_ANY_QUAL_CASCADED(unit, cmp_pair_p->first_key_info.qual_info, cascaded);
    if (cascaded == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Compare context cannot have qualifier with Cascaded input type.");
    }

    DNX_FIELD_IS_ANY_QUAL_CASCADED(unit, cmp_pair_p->second_key_info.qual_info, cascaded);
    if (cascaded == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Compare context cannot have qualifier with Cascaded input type.");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_compare_create(
    int unit,
    dnx_field_context_compare_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * cmp_pair_p)
{
    dnx_field_context_compare_info_t compare_info;
    int key_size;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_compare_create_verify
                           (unit, flags, stage, context_id, pair_id, cmp_pair_p));

    /**
     * Read the Partially filled sw-state. It includes configuration mode and key-ids for each mode per context.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));
    key_size = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size;

    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        /*
         * For single/double mode configure the qualifiers from cmp_pair_p->first_key_info for the key in
         * key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]
         */
        SHR_IF_ERR_EXIT(dnx_field_context_feature_key_set
                        (unit, flags, stage, context_id, DNX_FIELD_CONTEXT_FEATURE_COMPARE, key_size,
                         &(cmp_pair_p->first_key_info),
                         &compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]));

        /*
         * For a Double mode configure the qualifiers from
         * cmp_pair_p->first_key_info and cmp_pair_p->second_key_info
         * for the key in key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY] and
         * key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY] respectfully
         */
        if (compare_info.pair_1.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_feature_key_set
                            (unit, flags, stage, context_id, DNX_FIELD_CONTEXT_FEATURE_COMPARE, key_size,
                             &(cmp_pair_p->second_key_info),
                             &compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY]));
        }
        compare_info.pair_1.is_set = TRUE;
    }
    else if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        /*
         * For single/double mode configure the qualifiers from cmp_pair_p->first_key_info for the key in
         * key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]
         */
        SHR_IF_ERR_EXIT(dnx_field_context_feature_key_set
                        (unit, flags, stage, context_id, DNX_FIELD_CONTEXT_FEATURE_COMPARE, key_size,
                         &(cmp_pair_p->first_key_info),
                         &compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]));

        /*
         * For a Double mode configure the qualifiers from
         * cmp_pair_p->first_key_info and cmp_pair_p->second_key_info
         * for the key in key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY] and
         * key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY] respectfully
         */
        if (compare_info.pair_2.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_feature_key_set
                            (unit, flags, stage, context_id, DNX_FIELD_CONTEXT_FEATURE_COMPARE, key_size,
                             &(cmp_pair_p->second_key_info),
                             &compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY]));
        }
        compare_info.pair_2.is_set = TRUE;
    }

    /*
     * Save the complete compare sw-state. It includes mode configurations and key-ids for each mode per context,
     * and also the template for the key.
     */
    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.set(unit, context_id, &compare_info));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_compare_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * cmp_pair_p)
{
    dnx_field_context_compare_info_t compare_info;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Read the sw-state for context compare feature.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));

    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        dnx_field_context_hash_cmp_attach_info_set(unit,
                                                   &compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY],
                                                   &cmp_pair_p->first_key_info);
        dnx_field_context_hash_cmp_attach_info_set(unit,
                                                   &compare_info.pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY],
                                                   &cmp_pair_p->second_key_info);
    }
    else if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        dnx_field_context_hash_cmp_attach_info_set(unit,
                                                   &compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY],
                                                   &cmp_pair_p->first_key_info);
        dnx_field_context_hash_cmp_attach_info_set(unit,
                                                   &compare_info.pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY],
                                                   &cmp_pair_p->second_key_info);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_context_compare_destroy()
* \param [in] unit          - Device ID
* \param [in] flags         - TBD
* \param [in] stage         - Field Stage
* \param [in] context_id    - Field Context ID
*/
static shr_error_e
dnx_field_context_compare_destroy_verify(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    dnx_field_context_compare_info_t compare_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));
    if (stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Compare' can only be configured on iPMF1.\r\n");
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(DNX_FIELD_STAGE_IPMF1, context_id, 0);

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));

    /*
     * Verify that compare is enabled for at least one pair.
     */
    if ((compare_info.pair_1.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        && (compare_info.pair_2.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Compare is not enabled for context ID %d stage %s (%d).\r\n",
                     context_id, dnx_field_stage_text(unit, stage), stage);
    }

    /*
     * Verify if compare was set.
     */
    if ((compare_info.pair_1.is_set == FALSE) && (compare_info.pair_2.is_set == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Compare was not set for context ID %d stage %s (%d).\r\n",
                     context_id, dnx_field_stage_text(unit, stage), stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_compare_destroy(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id)
{
    dnx_field_context_compare_info_t compare_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_context_compare_destroy_verify(unit, flags, stage, context_id));

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &compare_info));
    /*
     * Mark the pair as unset.
     */
    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        if (compare_info.pair_1.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_feature_key_unset(unit, flags, stage, context_id,
                                                                &compare_info.
                                                                pair_1.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]));
            if (compare_info.pair_1.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_feature_key_unset(unit, flags, stage, context_id,
                                                                    &compare_info.pair_1.key_info
                                                                    [DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY]));
            }
        }
        compare_info.pair_1.is_set = FALSE;
    }
    else if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        if (compare_info.pair_2.mode != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_feature_key_unset(unit, flags, stage, context_id,
                                                                &compare_info.
                                                                pair_2.key_info[DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY]));
            if (compare_info.pair_2.mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_feature_key_unset(unit, flags, stage, context_id,
                                                                    &compare_info.pair_2.key_info
                                                                    [DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY]));
            }
        }
        compare_info.pair_2.is_set = FALSE;
    }

    /*
     * Save the sw state with the cleared template
     */
    SHR_IF_ERR_EXIT(DNX_FIELD_CONTEXT_IPMF1_INFO.compare_info.set(unit, context_id, &compare_info));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_name_to_context(
    int unit,
    dnx_field_stage_e stage,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_context_t * context_id_p)
{
    dnx_field_context_t context_id_ndx;
    char name_ndx[DBAL_MAX_STRING_LENGTH];
    int nof_context = dnx_data_field.stage.stage_info_get(unit, stage)->nof_contexts;
    int found = FALSE;
    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    SHR_NULL_CHECK(context_id_p, _SHR_E_PARAM, "context_id_p");

    if (stage == DNX_FIELD_STAGE_ACE || stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage not a PMF stage, received stage \"%s\".\r\n",
                     dnx_field_stage_text(unit, stage));
    }
    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }

    for (context_id_ndx = 0; context_id_ndx < nof_context; context_id_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, stage, context_id_ndx, &is_alloc));
        if (is_alloc)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, context_id_ndx, stage, name_ndx));
            if (0 == sal_strncmp(name, name_ndx, DBAL_MAX_STRING_LENGTH))
            {
                found = TRUE;
                break;
            }
        }
    }

    if (found)
    {
        (*context_id_p) = context_id_ndx;
    }
    else
    {
        (*context_id_p) = DNX_FIELD_CONTEXT_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_name_to_apptype(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    bcm_field_AppType_t * apptype_p)
{
    bcm_field_AppType_t apptype_ndx;
    int opcode_relative_index;
    char name_ndx[DBAL_MAX_STRING_LENGTH];
    int user_apptype_nof = dnx_data_field.external_tcam.apptype_user_nof_get(unit);
    int found = FALSE;
    shr_error_e rv;
    uint32 opcode_id;
    int max_nof_predef_apptypes;
    uint8 is_standard_image;
    uint8 acl_context;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    SHR_NULL_CHECK(apptype_p, _SHR_E_PARAM, "apptype_p");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }

    /*
     * Predefined apptypes
     */
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    if (is_standard_image)
    {
        max_nof_predef_apptypes = bcmFieldAppTypeCount;
    }
    else
    {
        max_nof_predef_apptypes = DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF;
    }
    for (apptype_ndx = 0; apptype_ndx < max_nof_predef_apptypes; apptype_ndx++)
    {
        rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, apptype_ndx, &opcode_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        if (0 == sal_strncmp(name, dnx_field_bcm_apptype_text(unit, apptype_ndx), DBAL_MAX_STRING_LENGTH))
        {
            found = TRUE;
            break;
        }
    }

    /*
     * User defined apptypes
     */
    if (found == FALSE)
    {
        for (opcode_relative_index = 0; opcode_relative_index < user_apptype_nof; opcode_relative_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                            acl_context.get(unit, opcode_relative_index, &acl_context));
            if (acl_context != DNX_FIELD_APPTYPE_SWSTATE_ACL_CONTEXT_INVALID)
            {
                SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                                name.stringget(unit, opcode_relative_index, name_ndx));
                if (0 == sal_strncmp(name, name_ndx, DBAL_MAX_STRING_LENGTH))
                {
                    SHR_IF_ERR_EXIT(dnx_field_map_opcode_dynamic_index_to_opcode
                                    (unit, opcode_relative_index, &opcode_id));
                    SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype_ndx));
                    found = TRUE;
                    break;
                }
            }
        }
    }

    if (found)
    {
        (*apptype_p) = apptype_ndx;
    }
    else
    {
        (*apptype_p) = DNX_FIELD_APPTYPE_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init default context id for all stages
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_context_default_context_init(
    int unit)
{
    dnx_field_stage_e stage;
    dnx_field_context_mode_t context_mode;
    dnx_field_context_t default_context = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Configure the context mode of the default contexts.
     * */
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    context_mode.context_ipmf1_mode.hash_mode = DNX_FIELD_CONTEXT_HASH_MODE_ENABLED;
    context_mode.context_ipmf1_mode.compare_mode_1 = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;

    sal_strncpy_s(context_mode.name, "Default_Ctx", sizeof(context_mode.name));
    /*
     * Create the default context for each stage.
     * Note we assume each stage with context has context selection.
     * also note that the iPMF2 default context is created by iPMF1.
     * For iFWD2 stage which is KBP stage the default contexts defined by Arch (same contexts as for iFWD1)
     */
    DNX_FIELD_STAGE_CS_QUAL_ITERATOR(stage)
    {
        if (stage != DNX_FIELD_STAGE_IPMF2 && stage != DNX_FIELD_STAGE_EXTERNAL)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, stage, &context_mode, &default_context));
        }
    }

        /**
     * set the Eth system Header to the default Context in iPMF1
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF1_CONTEXT_GENERAL, &entry_handle_id));
        /**
     * Set key and value fields to the table
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, default_context);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADER_PROFILE, INST_SINGLE,
                                 DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
     * Initialize iPMF3 default context with value of 1 Layer to remove
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF2_CONTEXT_GENERAL, &entry_handle_id));
        /**
     * Set key and value fields to the table
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, default_context);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_LAYERS_TO_REMOVE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init hash context for all action keys
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_context_hash_configuration_init(
    int unit)
{
    dnx_field_stage_e stage = DNX_FIELD_STAGE_IPMF1;
    int nof_contexts;
    uint32 entry_handle_id;
    uint32 crc_select_enum_iter;
    uint32 action_key_enum_iter;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_contexts = dnx_data_field.stage.stage_info_get(unit, stage)->nof_contexts;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_CONTEXT_HASH_CONFIGURATION_WITH_KEY, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, 0, nof_contexts - 1);

    /*
     * Since each CRC select machine can accept only one action key at all times,
     * we need to map each action key to a different CRC select for each context that can be configured as hash context
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                    (unit, DBAL_FIELD_CRC_SELECT, INVALID_ENUM, &crc_select_enum_iter));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                    (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, INVALID_ENUM, &action_key_enum_iter));
    while (action_key_enum_iter != INVALID_ENUM)
    {
        if (crc_select_enum_iter == INVALID_ENUM)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "There are less crc selects than action keys.\n");
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_SELECT, action_key_enum_iter,
                                     crc_select_enum_iter);

        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_CRC_SELECT, crc_select_enum_iter, &crc_select_enum_iter));
        SHR_IF_ERR_EXIT(dbal_fields_field_type_enum_next_enum_value_get
                        (unit, DBAL_FIELD_TYPE_DEF_FIELD_HASH_LB_KEY, action_key_enum_iter, &action_key_enum_iter));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_context_profile_map_kbp_context_init(
    int unit)
{
    uint32 opcode_id;
    uint32 profile_id;
    uint32 profile_id_get;
    unsigned int context_idx;
    uint8 acl_context_marked[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM] = { 0 };
    int nof_lkp_fwd_contexts;
    int nof_cs_fwd_contexts;
    int nof_acl_contexts;
    dnx_field_context_t lkp_fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t cs_fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    SHR_FUNC_INIT_VARS(unit);

    for (opcode_id = 0; opcode_id < DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF; opcode_id++)
    {
        uint8 opcode_is_valid;
        uint8 nof_contexts_per_opcode;
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.opcode_predef_info.is_valid.get(unit, opcode_id, &opcode_is_valid));
        if (opcode_is_valid == FALSE)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.opcode_predef_info.
                        nof_fwd2_context_ids.get(unit, opcode_id, &nof_contexts_per_opcode));
        if (nof_contexts_per_opcode <= 0)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_profile_get(unit, opcode_id, &profile_id));
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_lookup_fwd_contexts
                        (unit, opcode_id, &nof_lkp_fwd_contexts, lkp_fwd_contexts));
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_cs_fwd_contexts
                        (unit, opcode_id, &nof_cs_fwd_contexts, cs_fwd_contexts));

        /** Set the ACL context profile. */
        if (dnx_data_field.
            external_tcam.feature_get(unit, dnx_data_field_external_tcam_has_acl_context_profile_mapping))
        {
            int external_cs_based_on_fwd2 = dnx_data_field.features.external_cs_based_on_fwd2_get(unit);
            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_acl_contexts
                            (unit, opcode_id, external_cs_based_on_fwd2, &nof_acl_contexts, acl_contexts));
            for (context_idx = 0; context_idx < nof_acl_contexts; context_idx++)
            {
                if (acl_context_marked[acl_contexts[context_idx]])
                {
                    /** If we already set a profile for the ACL context, check that we do no have a profile mismatch.*/
                    SHR_IF_ERR_EXIT(dnx_field_context_apptype_acl_cs_profile_get
                                    (unit, acl_contexts[context_idx], &profile_id_get));
                    if (profile_id_get != profile_id)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Trying to rewrite CS profile %d as %d for ACL context %d.\n",
                                     profile_id_get, profile_id, acl_contexts[context_idx]);
                    }
                }
                else
                {
                    /** Write the profile*/
                    SHR_IF_ERR_EXIT(dnx_field_context_apptype_acl_cs_profile_set
                                    (unit, acl_contexts[context_idx], profile_id));
                }
                acl_context_marked[acl_contexts[context_idx]] = TRUE;
            }
        }
        for (context_idx = 0; context_idx < nof_lkp_fwd_contexts; context_idx++)
        {
            /** Only at init, we also write the FWD context profile for the predefined apptypes*/
            SHR_IF_ERR_EXIT(dnx_field_context_apptype_fwd_cs_profile_set
                            (unit, lkp_fwd_contexts[context_idx], profile_id));
        }
        for (context_idx = 0; context_idx < nof_cs_fwd_contexts; context_idx++)
        {
            /** Only at init, we also write the FWD context profile for the predefined apptypes*/
            SHR_IF_ERR_EXIT(dnx_field_context_apptype_fwd_cs_profile_ifwd_set
                            (unit, cs_fwd_contexts[context_idx], profile_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_default_context_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_context_hash_configuration_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The deinit function has nothing to deinit.
     */
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_default_sys_header_learn_ext(
    int unit,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE, &entry_handle_id));

    /**SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH is default profile for all packets*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADER_PROFILE,
                               DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEVER_ADD_PPH_LEARN_EXT, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**see H file: field_context.h */
shr_error_e
dnx_field_context_id_is_allocated(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    uint8 *is_allocated_p)
{
    bcm_field_AppType_t apptype;
    shr_error_e rv;
    uint32 opcode_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");
    opcode_id = context_id;

    if (stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        rv = dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype);
        if (rv == _SHR_E_NOT_FOUND)
        {
            *is_allocated_p = FALSE;
        }
        else
        {
            *is_allocated_p = TRUE;
            SHR_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated(unit, stage, context_id, is_allocated_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_context.h */
shr_error_e
dnx_field_context_acl_cpu_trap_code_profile_update(
    int unit,
    int trap_type,
    uint32 trap_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CPU_TRAP_CODE_PROFILE, &entry_handle_id));

    /** Set the keys. */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, trap_type);

    /** Request result field. */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_TRAP_CODE_PROFILE, INST_SINGLE, trap_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**see H file: field_context.h */
shr_error_e
dnx_field_context_acl_cpu_trap_code_profile_get(
    int unit,
    int trap_type,
    uint32 *trap_profile_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CPU_TRAP_CODE_PROFILE, &entry_handle_id));

    /** Set the keys. */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, trap_type);

    /** Request result field. */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_TRAP_CODE_PROFILE, INST_SINGLE, trap_profile_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See field_context.h */
shr_error_e
dnx_field_context_ifwd2_external_tcam_acl_init(
    int unit)
{
    int external_tcam_exists;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &external_tcam_exists));

    if (external_tcam_exists)
    {
        switch (dnx_data_field.external_tcam.type_get(unit))
        {
            case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
            {
#if defined(INCLUDE_KBP)
                SHR_IF_ERR_EXIT(dnx_field_kbp_context_ifwd2_kbp_acl_init(unit));
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

exit:
    SHR_FUNC_EXIT;
}
