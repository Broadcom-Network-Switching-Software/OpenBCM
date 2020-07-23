/** \file diag_dnx_field_qualifier.c
 *
 * Diagnostics procedures, for DNX, for 'qualifier' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include "diag_dnx_field_qualifier.h"
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Qualifier display (shell commands).
 */
static sh_sand_man_t Field_qualifier_user_list_man = {
    .brief = "'id'- displays the basic info about a specific field qualifier or range of qualifiers",
    .full = "'id' display for a range of field qualifiers. Range is specified via 'qualifier'.\r\n"
            "If no 'id' is specified then '0'-'nof_quals-1' is assumed (0-255).\r\n"
            "If only one value is specified for 'id' then this single value range is assumed.\r\n",
    .synopsis = "[id=<lower value>-<upper value>]",
    .examples = "id=0-20",
};

static sh_sand_man_t field_map_cs_man = {
    .brief = "Present bcm qualifiers valid for context selection",
    .full = "Present bcm qualifiers valid for context selection, per stage show DBAL field representing this qualifier",
    .synopsis = NULL,
    .examples = "\n" "name=ForwardingLayerIndex stage=ipmf1\n"
                "stage=ipmf1 show_sig=true\n"
                "stage=ipmf3 show_desc=false",
};

static sh_sand_man_t field_map_qual_man = {
    .brief = "Present bcm qualifiers valid for key construction",
    .full = "Present bcm qualifiers valid for key construction, per stage show DBAL field representing this qualifier",
    .synopsis = NULL,
    .examples = "\n" "name=InPort stage=IPMF1 class=meta",
};

static sh_sand_option_t field_map_cs_options[] = {
    {"name",  SAL_FIELD_TYPE_STR, "Filter qualifier by comparing this substring to the name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_ENUM, "Show only qualifier for the stage", "IPMF1", (void *)Field_qualifier_diag_stage_enum_table},
    {DIAG_DNX_FIELD_OPTION_SHOW_DESC, SAL_FIELD_TYPE_BOOL, "Show qualifiers description", "true"},
    {DIAG_DNX_FIELD_OPTION_SHOW_SIG, SAL_FIELD_TYPE_BOOL, "Show qualifiers signals", "false"},
    {NULL}
};

static sh_sand_option_t Field_qualifier_user_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest user defined qualifier ids to get its info",       "MIN-MAX",       (void *)Field_qualifier_enum_table, "MIN-MAX"},
    {NULL}
};

static sh_sand_option_t field_map_qualifier_bcm_options[] = {
    {"name",  SAL_FIELD_TYPE_STR,  "Filter qualifier by comparing this substring to the name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_UINT32, "Show only qualifier for the stage", "STAGE_LOWEST-STAGE_HIGHEST", (void *)Field_qualifier_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {"class", SAL_FIELD_TYPE_ENUM, "Filter qualifier by class (layer, header, meta)", "all", (void *)Field_qual_class_enum_table},
    {DIAG_DNX_FIELD_OPTION_SHOW_DESC, SAL_FIELD_TYPE_BOOL, "Show qualifiers description", "true"},
    {DIAG_DNX_FIELD_OPTION_SHOW_SIG, SAL_FIELD_TYPE_BOOL, "Show qualifiers signals", "false"},
    {NULL}
};

static sh_sand_option_t field_map_qualifier_dnx_options[] = {
    {"name",  SAL_FIELD_TYPE_STR,  "Filter qualifier by comparing this substring to the name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_UINT32, "Show only qualifier for the stage", "STAGE_LOWEST-STAGE_HIGHEST", (void *)Field_qualifier_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {"class", SAL_FIELD_TYPE_ENUM, "Filter qualifier by class (layer, header, meta)", "all", (void *)Field_qual_class_enum_table},
    {DIAG_DNX_FIELD_OPTION_SHOW_SIG, SAL_FIELD_TYPE_BOOL, "Show qualifiers signals", "false"},
    {NULL}
};


static sh_sand_option_t field_map_qualifier_virtualwire_options[] = {
    {DIAG_DNX_FIELD_OPTION_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest user defined virtualwire qualifier ids", "MIN-MAX", (void *)Field_qualifier_vw_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_VW_NAME,  SAL_FIELD_TYPE_STR,  "Filter virtualwire qualifier by comparing this substring to the virtualwire name", ""},
    {DIAG_DNX_FIELD_OPTION_STAGE, SAL_FIELD_TYPE_UINT32, "Show only virtualwire qualifier for the stage", "STAGE_LOWEST-STAGE_HIGHEST", (void *)Field_qualifier_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};


/*
 * }
 */
/* *INDENT-ON* */

static shr_error_e
diag_dnx_field_map_cs_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage;
    rhlist_t *cs_dnx_dump_list = NULL;
    cs_dnx_dump_t *cs_dnx_dump;
    char *match_n;
    int dnx_cs_qual_size;
    shr_error_e rv;
    const dnx_field_cs_qual_map_t *cs_qual_map_p = NULL;
    uint8 show_sig;
    uint8 show_desc;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_STAGE, stage);
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_SIG, show_sig);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_DESC, show_desc);

    PRT_TITLE_SET("CS Map");
    PRT_COLUMN_ADD("bcmQualifier");
    if (show_desc == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    }
    PRT_COLUMN_ADD("CS Field");
    PRT_COLUMN_ADD("Field Size");
    PRT_COLUMN_ADD("Raw");
    if (show_sig == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals");
    }
    PRT_COLUMN_ADD("Stage");

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                    (unit, Field_qualifier_diag_stage_enum_table[stage].string, &stage));
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dump(unit, &cs_dnx_dump_list));

    RHITERATOR(cs_dnx_dump, cs_dnx_dump_list)
    {
        if (!cs_dnx_dump->field_id[stage])
        {
            continue;
        }

        if (!ISEMPTY(match_n) && !sal_strcasestr(dnx_field_bcm_qual_text(unit, cs_dnx_dump->bcm_qual), match_n))
        {
            continue;
        }

        rv = dbal_tables_field_size_get(unit,
                                        dnx_field_map_stage_info[stage].cs_table_id,
                                        cs_dnx_dump->field_id[stage], TRUE, 0, 0, &dnx_cs_qual_size);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, cs_dnx_dump->bcm_qual));
        if (show_desc == TRUE)
        {
            char *qual_desc;
            /** Get description for specific bcmQualify. */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_description_get(unit, cs_dnx_dump->bcm_qual, &qual_desc));
            PRT_CELL_SET("%s", qual_desc);
        }
        PRT_CELL_SET("%s", dbal_field_to_string(unit, cs_dnx_dump->field_id[stage]));
        PRT_CELL_SET("%d", dnx_cs_qual_size);

        cs_qual_map_p = &dnx_field_map_stage_info[stage].cs_qual_map[cs_dnx_dump->bcm_qual];
        /**
         * In case current qualifier is marked as basic_object, we should present it.
         * Otherwise we should find the basic qualifier for current BCM qual.
         */
        if (cs_qual_map_p->flags & BCM_TO_DNX_BASIC_OBJECT)
        {
            PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, cs_dnx_dump->bcm_qual));
        }
        else
        {
            uint8 raw_qual_found = FALSE;
            cs_dnx_dump_t *cs_dnx_dump_raw;
            RHITERATOR(cs_dnx_dump_raw, cs_dnx_dump_list)
            {
                const dnx_field_cs_qual_map_t *cs_qual_map_raw_p = NULL;
                if (!cs_dnx_dump_raw->field_id[stage])
                {
                    continue;
                }

                cs_qual_map_raw_p = &dnx_field_map_stage_info[stage].cs_qual_map[cs_dnx_dump_raw->bcm_qual];

                if ((cs_qual_map_raw_p->flags & BCM_TO_DNX_BASIC_OBJECT) &&
                    (cs_dnx_dump->field_id[stage] == cs_dnx_dump_raw->field_id[stage]))
                {
                    PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, cs_dnx_dump_raw->bcm_qual));
                    raw_qual_found = TRUE;
                    break;
                }
            }

            /**
             * If no raw qualifier was found, it means that the current qualifier don't have
             * the flag BCM_TO_DNX_BASIC_OBJECT and in such case we present current BCM qual.
             */
            if (!raw_qual_found)
            {
                PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, cs_dnx_dump->bcm_qual));
            }
        }

        if (show_sig == TRUE)
        {
            const dnx_field_qual_map_t *meta_qual_map;
            dnx_field_qual_t dnx_qual;
            dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
            uint32 signal_iter;
            char qual_sig_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            rv = dnx_field_map_qual_bcm_to_dnx_int(unit, stage, FALSE, cs_dnx_dump->bcm_qual, &dnx_qual,
                                                   &meta_qual_map);
            if (rv != _SHR_E_PARAM && rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, stage, dnx_qual, qual_signals));
                for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
                {
                    char qual_sig_string_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                    /** Break in case we reach an empty string, which is indication the end of signals array. */
                    if (qual_signals[signal_iter].name == 0)
                    {
                        break;
                    }

                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s\n", qual_signals[signal_iter].name);
                    sal_snprintf(qual_sig_string_buff, sizeof(qual_sig_string_buff), "%s\n",
                                 qual_signals[signal_iter].name);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_sig_string, "%s", qual_sig_string_buff);
                    sal_strncat_s(qual_sig_string, qual_sig_string_buff, sizeof(qual_sig_string));
                }
            }

            if (sal_strncmp(qual_sig_string, "", sizeof(qual_sig_string)))
            {
                PRT_CELL_SET("%s", qual_sig_string);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }

        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

    PRT_COMMITX;

exit:
    if (cs_dnx_dump_list != NULL)
        utilex_rhlist_free_all(cs_dnx_dump_list);

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by context modes (compare and hashing).
 * \param [in] unit - The unit number.
 * \param [in] do_row_sep_none_add_p - Flag to indicate whether to print
 *  a row without separator.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] dnx_qual_id - DNX ID of the current qualifier.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] key_info_p - Key information about the current context mode, contains
 *  key_id, key_template and attach information.
 * \param [in] ctx_mode_name_p - Name of the current context mode, can be one
 *  of following (CMP1, CMP2 and HASH).
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_qualifier_ctx_mode_key_usage_cmp_hash_info_set(
    int unit,
    uint8 *do_row_sep_none_add_p,
    dnx_field_context_t context_id,
    dnx_field_qual_id_t dnx_qual_id,
    char *context_name_p,
    dnx_field_context_key_info_t * key_info_p,
    char *ctx_mode_name_p,
    prt_control_t * prt_ctr)
{
    uint32 qual_index;
    dnx_field_qual_t cs_qual;

    SHR_FUNC_INIT_VARS(unit);

    /** Iterate over all valid qualifiers for the current context mode. */
    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         key_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
    {
        cs_qual = DNX_QUAL_ID(key_info_p->key_template.key_qual_map[qual_index].qual_type);
        /** Get information about the current Field action. */
        if (cs_qual == dnx_qual_id)
        {
            if (*do_row_sep_none_add_p)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /**
                 * In case one action is used by more than one FG, we have to skip cells:
                 * Qualifier ID, Name, Size.
                 */
                PRT_CELL_SKIP(3);
            }

            if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
            {
                PRT_CELL_SET("CTX: %d / \"%s (%s)\"", context_id, context_name_p, ctx_mode_name_p);
            }
            else
            {
                PRT_CELL_SET("CTX: %d / \"N/A (%s)\"", context_id, ctx_mode_name_p);
            }

            *do_row_sep_none_add_p = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays FG IDs in which the specified by the caller Field qualifiers
 *   per qualifier ID or range of IDs  (optional) is used.
 * \param [in] unit - The unit number.
 * \param [in] dnx_qual_id - DNX ID of the current qualifier.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_qualifier_list_fg_ctx_usage_cb(
    int unit,
    dnx_field_qual_id_t dnx_qual_id,
    prt_control_t * prt_ctr)
{
    dnx_field_group_full_info_t *fg_info_p = NULL;
    uint8 is_fg_allocated;
    dnx_field_qual_t fg_qual;
    int max_nof_fg, fg_id_index, fg_qual_iter;
    uint8 do_row_sep_none_add;
    uint8 max_nof_cs_lines, ctx_id_index;
    uint8 ctx_is_alloc;
    char context_name[DBAL_MAX_STRING_LENGTH];
    dnx_field_context_mode_t context_mode;
    dnx_field_context_compare_info_t compare_info;
    dnx_field_context_hashing_info_t hashing_info;
    char ctx_mode_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    dnx_field_qual_class_e dnx_qual_class;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(fg_info_p, sizeof(dnx_field_group_full_info_t), "The FG full info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    max_nof_fg = dnx_data_field.group.nof_fgs_get(unit);
    max_nof_cs_lines = dnx_data_field.common_max_val.nof_contexts_get(unit);

    do_row_sep_none_add = FALSE;

    /** Iterate over maximum number of FGs. */
    for (fg_id_index = 0; fg_id_index < max_nof_fg; fg_id_index++)
    {
        /** Check if the FG is being allocated. */
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** If FG_ ID is not allocated, move to next FG ID. */
            continue;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_index, fg_info_p));

        /** Iterate over all quals in the FG and stop if the qual is being found. */
        for (fg_qual_iter = 0; fg_qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             fg_info_p->group_basic_info.dnx_quals[fg_qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID; fg_qual_iter++)
        {
            dnx_qual_class = DNX_QUAL_CLASS(fg_info_p->group_basic_info.dnx_quals[fg_qual_iter]);
            fg_qual = DNX_QUAL_ID(fg_info_p->group_basic_info.dnx_quals[fg_qual_iter]);
            /** Get information about the current Field action. */
            if ((fg_qual == dnx_qual_id) && (dnx_qual_class == DNX_FIELD_QUAL_CLASS_USER))
            {
                if (do_row_sep_none_add)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /**
                     * In case one action is used by more than one FG, we have to skip cells:
                     * Qualifier ID, Name, Size.
                     */
                    PRT_CELL_SKIP(3);
                }

                if (sal_strncmp(fg_info_p->group_basic_info.name, "", sizeof(fg_info_p->group_basic_info.name)))
                {
                    PRT_CELL_SET("FG: %d / \"%s\"", fg_id_index, fg_info_p->group_basic_info.name);
                }
                else
                {
                    PRT_CELL_SET("FG: %d / \"N/A\"", fg_id_index);
                }

                do_row_sep_none_add = TRUE;
                break;
            }
        }
    }

    /** Iterate over maximum number of FGs. */
    for (ctx_id_index = 0; ctx_id_index < max_nof_cs_lines; ctx_id_index++)
    {
        /** Check if the current Context ID is allocated. */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, ctx_id_index, &ctx_is_alloc));
        if (!ctx_is_alloc)
        {
            continue;
        }

        sal_memset(context_name, 0, sizeof(context_name));
        /** Take the name for current context from the SW state. */
        SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                        value.stringget(unit, ctx_id_index, DNX_FIELD_STAGE_IPMF1, context_name));

        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
        /** If the current context is allocated, get the information about it. */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, DNX_FIELD_STAGE_IPMF1, ctx_id_index, &context_mode));

        /** Get information for compare, hash, state table modes for the current context. */
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_key_usage_info_get
                        (unit, ctx_id_index, context_mode, &compare_info, &hashing_info));

        /** In case one of the compare pairs has valid mode, present its information, otherwise skip it. */
        if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
            context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
        {
            uint8 cmp_key_index;
            uint8 max_num_cmp_keys;
            uint8 cmp_pair_index;
            uint8 max_num_cmp_pairs;
            max_num_cmp_pairs = dnx_data_field.common_max_val.nof_compare_pairs_get(unit);
            max_num_cmp_keys = dnx_data_field.common_max_val.nof_compare_keys_in_compare_mode_get(unit);

            for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
            {
                for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
                {
                    if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                    {
                        sal_strncpy(ctx_mode_name, "CMP1", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                        SHR_IF_ERR_EXIT(diag_dnx_field_qualifier_ctx_mode_key_usage_cmp_hash_info_set
                                        (unit, &do_row_sep_none_add, ctx_id_index, dnx_qual_id, context_name,
                                         &compare_info.pair_1.key_info[cmp_key_index], ctx_mode_name, prt_ctr));
                    }
                    else
                    {
                        sal_strncpy(ctx_mode_name, "CMP2", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                        SHR_IF_ERR_EXIT(diag_dnx_field_qualifier_ctx_mode_key_usage_cmp_hash_info_set
                                        (unit, &do_row_sep_none_add, ctx_id_index, dnx_qual_id, context_name,
                                         &compare_info.pair_2.key_info[cmp_key_index], ctx_mode_name, prt_ctr));
                    }
                }
            }
        }

        /** In case hashing has valid mode, present its information, otherwise skip it. */
        if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
        {
            sal_strncpy(ctx_mode_name, "HASH", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
            SHR_IF_ERR_EXIT(diag_dnx_field_qualifier_ctx_mode_key_usage_cmp_hash_info_set
                            (unit, &do_row_sep_none_add, ctx_id_index, dnx_qual_id, context_name,
                             &hashing_info.key_info, ctx_mode_name, prt_ctr));
        }
    }

exit:
    SHR_FREE(fg_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field qualifiers per qualifier ID or range of IDs  (optional), specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_qualifier_user_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_qualify_t qual_id_lower, qual_id_upper;
    int do_display;
    dnx_field_qual_id_t dnx_qual_id;
    dnx_field_user_qual_info_t user_qual_info;
    bcm_field_qualify_t qual_id;
    bcm_field_qualifier_info_get_t qual_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, qual_id_lower, qual_id_upper);

    PRT_TITLE_SET("FIELD_User_Defined_Qualifiers");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifier ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG / Context (mode)");

    do_display = FALSE;
    for (qual_id = (BCM_FIELD_FIRST_USER_QUALIFIER_ID + qual_id_lower);
         qual_id <= (BCM_FIELD_FIRST_USER_QUALIFIER_ID + qual_id_upper); qual_id++)
    {
       /**
        * Check if the specified qualifier ID or rage of qualifier IDs are valid.
        */
        dnx_qual_id = qual_id - dnx_data_field.qual.user_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, dnx_qual_id, &user_qual_info));
        if (user_qual_info.valid == FALSE)
        {
            continue;
        }
        /** Get information about the current Field qualifier. */
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, qual_id, bcmFieldStageCount, &qual_info));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%d", dnx_qual_id);
       /**
        * Check if Field qualifier name is being provided, in case no set N/A as default value,
        * in other case set the provided name.
        */
        if (sal_strncmp((char *) (qual_info.name), "", sizeof(qual_info.name)))
        {
            PRT_CELL_SET("%s", qual_info.name);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        PRT_CELL_SET("%d", qual_info.size);

        SHR_IF_ERR_EXIT(diag_dnx_field_qualifier_list_fg_ctx_usage_cb(unit, dnx_qual_id, prt_ctr));

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated field qualifiers were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t field_map_qual_bcm_man = {
    .brief = "Present valid bcm qualifiers and the set of dnx qualifier IDs",
    .full = "Present valid bcm qualifiers and the set of dnx qualifier IDs",
    .synopsis = "[name=<bcm qualifier name>] [stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>] [class=<meta|layer|header>]\n",
    .examples = "\n" "name=InPort stage=ipmf1\n"
        "class=meta\n" "stage=ipmf1 show_sig=true\n" "stage=ipmf3 show_desc=false",
};

static shr_error_e
diag_dnx_field_map_qual_bcm_print(
    int unit,
    char *match_n,
    dnx_field_stage_e stage,
    dnx_field_qual_class_e in_class,
    uint8 show_sig,
    uint8 show_desc,
    prt_control_t * prt_ctr)
{
    dnx_field_qual_class_e class;
    uint32 dnx_qual_size;
    bcm_field_qualify_t bcm_qual;
    bcm_field_qualify_t bcm_qual_raw;
    dnx_field_qual_t dnx_qual;
    dnx_field_qual_t dnx_qual_raw;
    const dnx_field_qual_map_t *meta_qual_map;
    shr_error_e rv;

    
    bsl_severity_t original_severity_fld_proc = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    dnx_qual_raw = 0;

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        /** Don't start checking if the qualifier don't fit the filer. */
        if (!ISEMPTY(match_n) && !sal_strcasestr(dnx_field_bcm_qual_text(unit, bcm_qual), match_n))
        {
            continue;
        }

        rv = dnx_field_map_qual_bcm_to_dnx_int(unit, stage, FALSE, bcm_qual, &dnx_qual, &meta_qual_map);
        if (rv == _SHR_E_PARAM || rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        class = DNX_QUAL_CLASS(dnx_qual);
        if ((in_class != DNX_FIELD_QUAL_CLASS_NOF) && (class != in_class))
        {
            continue;
        }

        
        SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        
        rv = dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, &dnx_qual_size);
        
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        if (rv == _SHR_E_PARAM)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, bcm_qual));
        if (show_desc == TRUE)
        {
            char *qual_desc;
            /** Check if there is any description for specific bcmQualify. */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_description_get(unit, bcm_qual, &qual_desc));
            PRT_CELL_SET("%s", qual_desc);
        }
        PRT_CELL_SET("%d", dnx_qual_size);
        PRT_CELL_SET("%s", dnx_field_qual_class_text(class));
        /**
         * In case current qualifier is marked as basic_object, we should present it.
         * Otherwise we should find the basic qualifier for current BCM qual.
         */
        if (meta_qual_map->flags & BCM_TO_DNX_BASIC_OBJECT)
        {
            PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, bcm_qual));
        }
        else
        {
            uint8 raw_qual_found = FALSE;
            for (bcm_qual_raw = 0; bcm_qual_raw < bcmFieldQualifyCount; bcm_qual_raw++)
            {
                rv = dnx_field_map_qual_bcm_to_dnx_int(unit, stage, FALSE, bcm_qual_raw, &dnx_qual_raw, &meta_qual_map);
                if (rv == _SHR_E_PARAM || rv == _SHR_E_NOT_FOUND)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(rv);
                if ((meta_qual_map->flags & BCM_TO_DNX_BASIC_OBJECT) && (dnx_qual == dnx_qual_raw))
                {
                    PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, bcm_qual_raw));
                    raw_qual_found = TRUE;
                    break;
                }
            }

            /**
             * If no raw qualifier was found, it means that the current qualifier don't have
             * the flag BCM_TO_DNX_BASIC_OBJECT and in such case we present current BCM qual.
             */
            if (!raw_qual_found)
            {
                PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, bcm_qual));
            }
        }
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, dnx_qual));

        if (show_sig == TRUE)
        {
            dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
            uint32 signal_iter;
            char qual_sig_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, stage, dnx_qual, qual_signals));
            for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
            {
                char qual_sig_string_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                /** Break in case we reach an empty string, which is indication the end of signals array. */
                if (qual_signals[signal_iter].name == 0)
                {
                    break;
                }

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s\n", qual_signals[signal_iter].name);
                sal_snprintf(qual_sig_string_buff, sizeof(qual_sig_string_buff), "%s\n",
                             qual_signals[signal_iter].name);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_sig_string, "%s", qual_sig_string_buff);
                sal_strncat_s(qual_sig_string, qual_sig_string_buff, sizeof(qual_sig_string));
            }

            if (sal_strncmp(qual_sig_string, "", sizeof(qual_sig_string)))
            {
                PRT_CELL_SET("%s", qual_sig_string);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }
        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_map_qual_bcm_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    dnx_field_qual_class_e in_class;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    uint8 show_sig;
    uint8 show_desc;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_ENUM("class", in_class);
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_SIG, show_sig);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_DESC, show_desc);

    PRT_TITLE_SET("Qualifier Map");
    PRT_COLUMN_ADD("bcmQualifer");
    if (show_desc == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    }
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Class");
    PRT_COLUMN_ADD("Raw");
    PRT_COLUMN_ADD("DNX Qualifier");
    if (show_sig == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals");
    }
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_qualifier_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_map_qual_bcm_print
                        (unit, match_n, field_stage, in_class, show_sig, show_desc, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t field_map_qual_dnx_man = {
    .brief = "Present valid dnx qualifiers info",
    .full = "Present valid dnx qualifiers and their parameters",
    .synopsis = "[name=<dnx qualifier name>] [stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>] [class=<meta|layer|header>]\n",
    .examples = "\n" "name=IN_PORT stage=ipmf1 class=meta\n" "name=IN_PORT stage=ipmf1 show_sig=true",
};

static shr_error_e
diag_dnx_field_map_qual_dnx_print(
    int unit,
    char *match_n,
    dnx_field_stage_e stage,
    dnx_field_qual_class_e in_class,
    uint8 show_sig,
    prt_control_t * prt_ctr)
{
    int i_qual;
    dnx_field_qual_t *dnx_qual_list = NULL;
    int dnx_field_qual_num;
    CONST char *dnx_qual_name;
    uint32 dnx_qual_size;
    int dnx_qual_offset;
    bcm_field_qualify_t bcm_qual;
    dnx_field_qual_class_e class;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_list(unit, stage, &dnx_qual_list, &dnx_field_qual_num));

    for (i_qual = 0; i_qual < dnx_field_qual_num; i_qual++)
    {
        dnx_qual_name = dnx_field_dnx_qual_text(unit, dnx_qual_list[i_qual]);
        class = DNX_QUAL_CLASS(dnx_qual_list[i_qual]);

        if (!ISEMPTY(match_n) && !sal_strcasestr(dnx_qual_name, match_n))
            continue;

        if ((in_class != DNX_FIELD_QUAL_CLASS_NOF) && (class != in_class))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual_list[i_qual], &dnx_qual_size));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, stage, dnx_qual_list[i_qual], &dnx_qual_offset));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm(unit, stage, dnx_qual_list[i_qual], &bcm_qual));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_qual_name);
        PRT_CELL_SET("%s", dnx_field_qual_class_text(class));
        PRT_CELL_SET("%d", dnx_qual_size);
        PRT_CELL_SET("%d", dnx_qual_offset);
        if (bcm_qual != DNX_BCM_ID_INVALID)
        {
            PRT_CELL_SET("%s", dnx_field_bcm_qual_text(unit, bcm_qual));
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        if (show_sig == TRUE)
        {
            dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
            uint32 signal_iter;
            char qual_sig_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, stage, dnx_qual_list[i_qual], qual_signals));
            for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
            {
                char qual_sig_string_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                /** Break in case we reach an empty string, which is indication the end of signals array. */
                if (qual_signals[signal_iter].name == 0)
                {
                    break;
                }

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s\n", qual_signals[signal_iter].name);
                sal_snprintf(qual_sig_string_buff, sizeof(qual_sig_string_buff), "%s\n",
                             qual_signals[signal_iter].name);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_sig_string, "%s", qual_sig_string_buff);
                sal_strncat_s(qual_sig_string, qual_sig_string_buff, sizeof(qual_sig_string));
            }

            if (sal_strncmp(qual_sig_string, "", sizeof(qual_sig_string)))
            {
                PRT_CELL_SET("%s", qual_sig_string);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }

        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    if (dnx_qual_list != NULL)
        sal_free(dnx_qual_list);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_map_qual_dnx_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    dnx_field_qual_class_e in_class;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    uint8 show_sig;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the inputs from Shell. */
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_ENUM("class", in_class);
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_SHOW_SIG, show_sig);

    PRT_TITLE_SET("DNX Qualifier List");
    PRT_COLUMN_ADD("Field Name");
    PRT_COLUMN_ADD("Class");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Offset");
    PRT_COLUMN_ADD("BCM");
    if (show_sig == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals");
    }
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_qualifier_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_map_qual_dnx_print(unit, match_n, field_stage, in_class, show_sig, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t field_map_qual_virtualwire_man = {
    .brief = "Present valid virtualwire qualifiers info",
    .full = "Present valid virtualwire qualifiers and their parameters",
    .synopsis = "[vw_name=<virtualwire name>] [stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>] [id=<virtualwire id>]\n",
    .examples = "\n" "vw_name= stage=ipmf1 id=8292-8390",
};

static shr_error_e
diag_dnx_field_qualifier_virtualwire_print(
    int unit,
    uint32 qual_id_lower,
    uint32 qual_id_upper,
    char *vw_match_n,
    dnx_field_stage_e stage,
    prt_control_t * prt_ctr)
{
    int bcm_vw_qual_id;
    uint32 dnx_qual_size;
    const dnx_field_qual_map_t *meta_qual_map;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_vw_qual_id = qual_id_lower; bcm_vw_qual_id <= qual_id_upper; bcm_vw_qual_id++)
    {
        dnx_field_qual_t dnx_vw_qual;
        CONST char *vw_name;
        shr_error_e rv =
            dnx_field_map_qual_bcm_to_dnx_int(unit, stage, FALSE, bcm_vw_qual_id, &dnx_vw_qual, &meta_qual_map);
        if (rv == _SHR_E_PARAM || rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        vw_name = dnx_field_dnx_qual_text(unit, dnx_vw_qual);

        if (!ISEMPTY(vw_match_n) && !sal_strcasestr(vw_name, vw_match_n))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, stage, dnx_vw_qual, &dnx_qual_size));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", bcm_vw_qual_id);
        PRT_CELL_SET("%s", vw_name);
        PRT_CELL_SET("%d", dnx_qual_size);
        PRT_CELL_SET("%s", dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_qualifier_virtualwire_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *vw_match_n;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    uint32 qual_id_lower, qual_id_upper;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the inputs from Shell. */
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_ID, qual_id_lower, qual_id_upper);
    SH_SAND_GET_STR(DIAG_DNX_FIELD_OPTION_VW_NAME, vw_match_n);

    PRT_TITLE_SET("Virtual Wire Qualifier Map");
    PRT_COLUMN_ADD("Qualifier ID");
    PRT_COLUMN_ADD("VW Name");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_qualifier_diag_stage_enum_table[stage_index].string, &field_stage));

        SHR_IF_ERR_EXIT(diag_dnx_field_qualifier_virtualwire_print
                        (unit, qual_id_lower, qual_id_upper, vw_match_n, field_stage, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/*
 * }
 */


/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'qualifier' shell commands (bcm, dnx)
 */

static sh_sand_cmd_t sh_dnx_field_qualifier_cmds[] = {
    {"BCM",      diag_dnx_field_map_qual_bcm_cb,        NULL, field_map_qualifier_bcm_options,          &field_map_qual_bcm_man},
    {"DNX",      diag_dnx_field_map_qual_dnx_cb,        NULL, field_map_qualifier_dnx_options,          &field_map_qual_dnx_man},
    {NULL}
};

sh_sand_cmd_t Sh_dnx_field_qualifier_cmds[] = {
    {"ContextSelect", diag_dnx_field_map_cs_cb, NULL,  field_map_cs_options,      &field_map_cs_man},
    {"PReDefined",     NULL,            sh_dnx_field_qualifier_cmds, NULL, &field_map_qual_man},
    {"USeR",     diag_dnx_field_qualifier_user_list_cb, NULL, Field_qualifier_user_list_options,    &Field_qualifier_user_list_man},
    {"VirtualWire",     diag_dnx_field_qualifier_virtualwire_cb, NULL, field_map_qualifier_virtualwire_options,    &field_map_qual_virtualwire_man},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
