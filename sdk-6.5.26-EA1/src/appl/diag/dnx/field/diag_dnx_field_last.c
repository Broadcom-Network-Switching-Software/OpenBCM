/** \file diag_dnx_field_last.c
 *
 * Diagnostics procedures, for DNX, for 'last' (last packet) operations.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/sand/sand_signals.h>
#include <bcm/instru.h>
#include <src/bcm/dnx/field/map/field_map_local.h>
#include "diag_dnx_field_utils.h"
#include "diag_dnx_field_last.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#endif

/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>

/*
 * }
 */
static bcm_field_group_t tcam_fg_id = BCM_FIELD_ID_INVALID;
static bcm_field_group_t exem_fg_id = BCM_FIELD_ID_INVALID;

/*
 * DEFINEs
 * {
 */
/**
 * Retrieve the value of the 'tcam_cs_hit_bit_support' from DNX-DATA to be used for checking
 * if current device is supporting the TCAM Context Selection HIT Indication.
 * Will be used as flag in the diagnostic.
 * This flag will be used, because on some of the devices (For Example: JR2, JR2_B0)
 * the TCAM CS HIT indication memories will be not supported, and this flag will avoid reading of the memory.
 */
#define DIAG_DNX_FIELD_LAST_TCAM_HIT_INDICATION_ENABLE  dnx_data_field.diag.feature_get(unit, dnx_data_field_diag_tcam_cs_hit_bit_support)

#ifdef ADAPTER_SERVER_MODE
#define DIAG_DNX_FIELD_LAST_IS_SIM TRUE
#else
#define DIAG_DNX_FIELD_LAST_IS_SIM FALSE
#endif

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

/*
 * }
 */

extern uint8 dbal_fields_is_field_encoded(
    int unit,
    dbal_fields_e field_id);

/**
 * \brief
 *   This function allocates some resources in ePMF
 *   so that key A and C will be used, and key B won't.
 *   for testing during the 'field last info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_epmf_keys_a_c(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    SHR_FUNC_INIT_VARS(unit);

    tcam_fg_id = 71;
    exem_fg_id = 72;

    /*
     * Add EXEM field group to occupy key A
     */
    bcm_field_group_info_t_init(&fg_info);
    sal_strncpy_s((char *) (fg_info.name), "field_last_info_diag_epmf_key_A", sizeof(fg_info.name));
    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyDstMac;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatId0;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &exem_fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, exem_fg_id, 0 /* default context */ , &attach_info));

    /*
     * Add second TCAM field group to occupy key C
     */
    bcm_field_group_info_t_init(&fg_info);
    sal_strncpy_s((char *) (fg_info.name), "field_last_info_diag_epmf_key_C", sizeof(fg_info.name));
    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatId1;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &tcam_fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, tcam_fg_id, 0 /* default context */ , &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function allocates some resources to be used
 *   for testing during the 'field last info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Attach field groups to keys A and C on ePMF
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_last_epmf_keys_a_c(unit));
    /*
     * Send ITMH packet
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_send_itmh_packet(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function allocates some resources in ePMF
 *   so that key A and C will be used, and key B won't.
 *   for testing during the 'field last info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_epmf_keys_a_c_destroy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, tcam_fg_id, 0 /* default context */ ));
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, exem_fg_id, 0 /* default context */ ));

    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, tcam_fg_id));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, exem_fg_id));

    tcam_fg_id = BCM_FIELD_ID_INVALID;
    exem_fg_id = BCM_FIELD_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function deallocates the resources that used
 *   for testing during the 'field last info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Attach field groups to keys A and C on ePMF
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_last_epmf_keys_a_c_destroy(unit));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_last_fg_info_t_init(
    int unit,
    diag_dnx_field_last_fg_info_t * last_fg_info_p)
{
    uint32 max_nof_fgs = dnx_data_field.group.nof_fgs_get(unit);
    uint32 fg_id_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(last_fg_info_p, _SHR_E_PARAM, "last_fg_info_p");

    sal_memset((void *) last_fg_info_p, 0, sizeof(*last_fg_info_p));

    for (fg_id_index = 0; fg_id_index < max_nof_fgs; fg_id_index++)
    {
        sal_memset(last_fg_info_p[fg_id_index].key_ids, DNX_FIELD_KEY_ID_INVALID,
                   sizeof(last_fg_info_p[fg_id_index].key_ids));
        last_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns the presel ID which was hit.
 * \param [in] unit - The unit number.
 * \param [in] cs_hit_indic_table - Relevant CS hit indication DBAL table per stage.
 * \param [out] presel_id - Presel id, which was hit, to be returned as result of the
 *  function.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_hit_info_get(
    int unit,
    dbal_tables_e cs_hit_indic_table,
    bcm_field_presel_t * presel_id)
{
    uint8 cs_line_id, cs_entry_id;
    uint8 max_nof_cs_lines, max_nof_entries_per_line;
    uint8 hit_indication_value;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get max values from DNX data. */
    max_nof_entries_per_line = dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit);
    max_nof_cs_lines = (dnx_data_field.common_max_val.nof_cs_lines_get(unit) / max_nof_entries_per_line);

    /** Initialize the presel_id to its default value. */
    *presel_id = DNX_FIELD_PRESEL_ID_INVALID;
    hit_indication_value = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cs_hit_indic_table, &entry_handle_id));

    /** Iterate over the context selection lines. */
    for (cs_line_id = 0; cs_line_id < max_nof_cs_lines; cs_line_id++)
    {
        /** Iterate over the presel ID in the 8b bitmap. */
        for (cs_entry_id = 0; cs_entry_id < max_nof_entries_per_line; cs_entry_id++)
        {
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_ROW_IDX, cs_line_id);
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_IDX, cs_entry_id);
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TCAM_HIT, INST_SINGLE, &hit_indication_value);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            /** In case a hit was found, return the presel ID. */
            if (hit_indication_value)
            {
                *presel_id = ((cs_line_id * max_nof_entries_per_line) + cs_entry_id);
                break;
            }
        }
        if (*presel_id != DNX_FIELD_PRESEL_ID_INVALID)
        {
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about qualifiers of the
 *   by context modes (compare and hashing) used keys.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] is_hash - Flag to indicate if the function was called
 *  to present hash qualifier info. Can be TRUE (hash) or FALSE (cmp).
 * \param [in] cmp_key_index - Compare key index.
 * \param [in] cmp_pair_index - Compare pair index.
 * \param [in] context_id - ID of the selected context.
 * \param [in] key_info_p - Key information about the current context mode, contains
 *  key_id, key_template and attach information.
 * \param [in] new_cmp_mode_id_print_p - Flag to indicate if we have to print,
 *  a new compare mode on different row, with separator.
 * \param [out] do_display_any_key_info_p - Flag to be set to TRUE in case any info
 *  about keys was set. Default value is FALSE.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_cmp_hash_qual_info_set(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    uint8 is_hash,
    uint8 cmp_key_index,
    uint8 cmp_pair_index,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * key_info_p,
    uint8 *new_cmp_mode_id_print_p,
    uint8 *do_display_any_key_info_p,
    prt_control_t * prt_ctr)
{
    uint32 qual_index;
    uint8 key_index;

    SHR_FUNC_INIT_VARS(unit);

    /** In compare and HASH modes we can have up to two keys, iterate over them. */
    for (key_index = 0; key_index < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
         key_info_p->key_id.id[key_index] != DNX_FIELD_KEY_ID_INVALID; key_index++)
    {
        /** Iterate over all valid qualifiers for the current context mode. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             key_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {

            /** In case we have more than one valid qualifiers for the current key, we have to skip some cells. */
            if (!qual_index)
            {
                /** In case of HASH we print the key ID on first line of the table. */
                if (is_hash)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                }
                else
                {
                    /**
                     * In case the flag new_cmp_mode_id_print_p is set to TRUE,
                     * we will print cmp mode ID, cmp pair ID and the key ID on
                     * first line of the table. Otherwise we skip the cmp mode ID
                     * cell.
                     */
                    if (*new_cmp_mode_id_print_p)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        /** Set 'CMP Mode Id' cell. */
                        PRT_CELL_SET("%d", (cmp_pair_index + 1));
                        /** Set 'Pair Id' cell. */
                        PRT_CELL_SET("%d", (cmp_key_index + 1));
                        /** Set 'Initial Key Id' cell. */
                        PRT_CELL_SET("%c", ('A' + key_info_p->key_id.id[key_index]));

                        *new_cmp_mode_id_print_p = FALSE;
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        /** Skip 'CMP Mode Id' cell. */
                        PRT_CELL_SKIP(1);
                        /** Set 'Pair Id' cell. */
                        PRT_CELL_SET("%d", (cmp_key_index + 1));
                        /** Set 'Initial Key Id' cell. */
                        PRT_CELL_SET("%c", ('A' + key_info_p->key_id.id[key_index]));
                    }
                }
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                if (!is_hash)
                {
                    /**
                     * In case of CMP and we have more than one qualifier,
                     * we should skip the cells for the CMP Mode Id, Pair Id and Key Id.
                     */
                    PRT_CELL_SKIP(3);
                }
            }

            /** Set 'Qual Type' cell. */
            PRT_CELL_SET("%s",
                         dnx_field_dnx_qual_text(unit, key_info_p->key_template.key_qual_map[qual_index].qual_type));
            /** Set 'Qual LSB' cell. */
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].lsb);
            /** Set 'Qual Size' cell. */
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].size);

            *do_display_any_key_info_p = TRUE;
        }

        /**
         * For HASH we are not dividing key info table per key,
         * all the used qualifiers will be present for both keys.
         */
        if (is_hash)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about, keys used by the context modes.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] context_mode - Context mode info (cmp, hash).
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_mode_key_info_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t context_mode,
    char *block_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_compare_info_t compare_info;
    dnx_field_context_hashing_info_t hashing_info;
    uint8 do_display_any_key_info;
    uint8 new_cmp_mode_id_print;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    do_display_any_key_info = FALSE;

    /** Get information for compare, hash, state table modes for the current context. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_key_usage_info_get
                    (unit, context_id, context_mode, &compare_info, &hashing_info));

    /** In case one of the compare pairs has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
        context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        uint8 cmp_key_index;
        uint8 max_num_cmp_keys;
        uint8 cmp_pair_index;
        uint8 max_num_cmp_pairs;
        max_num_cmp_pairs = dnx_data_field.compare.nof_compare_pairs_get(unit);
        max_num_cmp_keys = dnx_data_field.compare.nof_keys_get(unit);

        PRT_TITLE_SET("Compare Key Info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CMP Mode Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Pair Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Initial Key Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");

        for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
        {
            new_cmp_mode_id_print = TRUE;

            for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
            {
                if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                                    (unit, core_id, block_p, FALSE, cmp_key_index, cmp_pair_index, context_id,
                                     &compare_info.pair_1.key_info[cmp_key_index],
                                     &new_cmp_mode_id_print, &do_display_any_key_info, prt_ctr));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                                    (unit, core_id, block_p, FALSE, cmp_key_index, cmp_pair_index, context_id,
                                     &compare_info.pair_2.key_info[cmp_key_index],
                                     &new_cmp_mode_id_print, &do_display_any_key_info, prt_ctr));
                }
            }
        }
    }

    if (do_display_any_key_info)
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
    }

    do_display_any_key_info = FALSE;

    /** In case hashing has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        PRT_TITLE_SET("Hash Key Info of initial keys 'I' and 'J'");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");

        SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                        (unit, core_id, block_p, TRUE, FALSE, FALSE, context_id,
                         &hashing_info.key_info, FALSE, &do_display_any_key_info, prt_ctr));
    }

    if (do_display_any_key_info)
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
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

char *
diag_dnx_field_last_bcm_packet_remove_layer_description(
    bcm_field_packet_remove_layers_t bcm_packet_remove_layer)
{
    char *returned_description;

    switch (bcm_packet_remove_layer)
    {
        case bcmFieldPacketRemoveLayerOffset0:
        {
            returned_description = "0";
            break;
        }
        case bcmFieldPacketRemoveLayerOffset1:
        {
            returned_description = "layer_offset[0]";
            break;
        }
        case bcmFieldPacketRemoveLayerForwardingOffset0:
        {
            returned_description = "layer_offset[FWD]";
            break;
        }
        case bcmFieldPacketRemoveLayerForwardingOffset1:
        {
            returned_description = "layer_offset[FWD+1]";
            break;
        }
        default:
        {
            returned_description = "Invalid bcmFieldPacketRemoveLayer enum";
            break;
        }
    }

    return returned_description;
}
/**
 * \brief
 *   This function presents information about, which context was hit and relevant
 *   presel ID.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] names_p - Names of the blocks for signals.
 * \param [in] one_pkt - if the user commited that only one packet was injected, additional info will be print.
 * \param [in] visibility_mode - Visibility mode.
 * \param [out] context_id_p - ID of the selected context.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    diag_dnx_field_utils_sig_block_names_t * names_p,
    uint8 one_pkt,
    bcm_instru_vis_mode_control_t visibility_mode,
    dnx_field_context_t * context_id_p,
    sh_sand_control_t * sand_control)
{
    uint32 context_id;
    char context_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    bcm_field_stage_t bcm_stage;

    bcm_field_presel_t hit_presel_id;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    uint8 is_apptype_static = FALSE;
    dnx_field_context_apptype_info_t apptype_info;
    bcm_field_AppType_t apptype;
    char base_apptype_name[DBAL_MAX_STRING_LENGTH] = { 0 };
#endif
    char *block_p = names_p->block;
    char *from_p = names_p->stage;
    char *to_p = names_p->to;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (one_pkt == FALSE && visibility_mode != bcmInstruVisModeAlways && field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        LOG_CLI_EX("\r\n"
                   "Context info for IPMF3 stage can be presented only if visibility mode is always!!!%s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    PRT_TITLE_SET("Context Selection Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel Id");

    /** Read the signal of 'Context' to retrieve the selected context id. */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, block_p, from_p, to_p, "Context", &context_id, 1));
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * For iPMF1, the context indicated by the signal is the context of iPMF2. Need to find out the parent context
         * to deduce the iPMF1 context.
         */
        dnx_field_context_t ipmf1_context;
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, DNX_FIELD_STAGE_IPMF2, context_id, &ipmf1_context));
        context_id = ipmf1_context;
    }

    *context_id_p = (dnx_field_context_t) context_id;

    sal_memset(context_name, 0, sizeof(context_name));
    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
        uint32 acl_context = 0;
        uint32 opcode_id;
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, block_p, from_p, to_p, "ACL_Context", &acl_context, 1));
        
        SHR_IF_ERR_EXIT(dnx_field_map_contexts_to_apptype_get(unit, *context_id_p, acl_context, &apptype));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));
        /** Initialize the apptype structure. */
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, &apptype_info));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_get(unit, apptype, &apptype_info));
        if (apptype_info.base_static_apptype == DNX_FIELD_APPTYPE_INVALID)
        {
            is_apptype_static = TRUE;
        }
        else
        {
            sal_strncpy_s(base_apptype_name, dnx_field_bcm_apptype_text(unit, apptype_info.base_static_apptype),
                          sizeof(base_apptype_name));
        }
        sal_strncpy_s(context_name, apptype_info.name, sizeof(context_name));

        *context_id_p = opcode_id;
#else
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\r\n");
#endif
    }
    else
    {
        /** Take the name for current context from the SW state. */
        SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                        value.stringget(unit, *context_id_p, field_stage, context_name));
    }

    /** Convert DNX to BCM Field Stage. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    /** Check if the CS TCAM HIT indication is supported on current device. */
    if (DIAG_DNX_FIELD_LAST_TCAM_HIT_INDICATION_ENABLE)
    {

        /** Get the presel ID which was hit. */
        SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_hit_info_get
                        (unit, dnx_field_map_stage_info[field_stage].cs_hit_indication_table_id, &hit_presel_id));

        if (hit_presel_id != DNX_FIELD_PRESEL_ID_INVALID)
        {
            /** Get information about preselector, which has being set to the current context. */
            bcm_field_presel_entry_id_info_init(&entry_id);
            bcm_field_presel_entry_data_info_init(&entry_data);
            entry_id.presel_id = hit_presel_id;
            entry_id.stage = bcm_stage;
            /** Retrieve information about the current presel_id. */
            SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
            /**
             * Check if the retrieved presel id is valid for the selected context.
             */
            if ((entry_data.entry_valid) && (entry_data.context_id == *context_id_p))
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", hit_presel_id);
            }
            else
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
            }
        }
        else
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
        }
    }
    else
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%s", "Context Id/Name");

    /**
    * Check if Context name is being provided, in case no set N/A as default value,
    * in other case set the provided name.
    */
    if (sal_strncmp(context_name, "", sizeof(context_name)))
    {
        if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            if (is_apptype_static)
            {
                PRT_CELL_SET("%d (AT)/\"%s\"", apptype, context_name);
            }
            else
            {
                PRT_CELL_SET("%d (AT)/\"%s\" (FWD: %d/\"%s\"", apptype, context_name, apptype_info.base_static_apptype,
                             base_apptype_name);
            }
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\r\n");
#endif
        }
        else
        {
            PRT_CELL_SET("%d /\"%s\"", *context_id_p, context_name);
        }
    }
    else
    {
        if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            if (is_apptype_static)
            {
                PRT_CELL_SET("%d (AT)/\"%s\"", apptype, "N/A");
            }
            else
            {
                PRT_CELL_SET("%d (AT)/\"%s\" (FWD: %d/\"%s\"", apptype, "N/A", apptype_info.base_static_apptype,
                             base_apptype_name);
            }
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\r\n");
#endif
        }
        else
        {
            PRT_CELL_SET("%d/\"%s\"", *context_id_p, "N/A");
        }
    }

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        dnx_field_context_mode_t context_mode;
        uint32 sys_prof_index;
        /** Read the signal of 'Sys_Header_Profile_Index' to retrieve its value. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, block_p, from_p, to_p, "Sys_Header_Profile_Index", &sys_prof_index, 1));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "System Header Profile");
        PRT_CELL_SET("%s", dnx_data_field.system_headers.system_header_profiles_get(unit, sys_prof_index)->name);

        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
        /** If the current context is allocated, get the information about it. */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &context_mode));

        /** In case one of the context modes CMP or HASH has valid mode, present its information, otherwise skip it. */
        if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
            context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
            context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
        {
            char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_context_modes_string_get(unit, context_mode, mode_string));

            /** Add new row, to present the context modes. */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Mode");
            PRT_CELL_SET("%s", mode_string);
            PRT_COMMITX;

            /** Print the key info for the different context modes: CMP, HASH. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_mode_key_info_print(unit, core_id, context_id,
                                                                       context_mode, block_p, sand_control));
        }
        else
        {
            PRT_COMMITX;
        }
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        uint32 bytes_to_remove_hdr_sig_val;
        uint32 bytes_to_remove;
        uint32 layer_to_remove;
        /** Read the signal of 'Bytes_to_Remove_Header' to retrieve its value. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "IRPP", "IPMF3", "LBP", "Bytes_to_Remove_Header", &bytes_to_remove_hdr_sig_val,
                         1));

        layer_to_remove = bytes_to_remove_hdr_sig_val & dnx_data_field.diag.layers_to_remove_mask_get(unit);
        bytes_to_remove =
            (bytes_to_remove_hdr_sig_val >> dnx_data_field.
             diag.layers_to_remove_size_in_bit_get(unit)) & dnx_data_field.diag.bytes_to_remove_mask_get(unit);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Bytes to Remove");
        PRT_CELL_SET("%s + %d bytes", diag_dnx_field_last_bcm_packet_remove_layer_description(layer_to_remove),
                     bytes_to_remove);

        PRT_COMMITX;
    }
    else
    {
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function retrieve information about last hit entry per TCAM field group.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id_index - Iterator value to know, on which place to save the entry id.
 * \param [in] fg_id - Field group ID, which performed lookups and have a hit.
 * \param [out] last_diag_fg_info_p - Stores entry which was hit per FG.
 * \param [out] is_entry_found_p - Indication if we find at least one entry.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_tcam_entry_info_get(
    int unit,
    bcm_core_t core_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *is_entry_found_p)
{
    uint32 max_nof_tcam_entries;
    uint8 hit_indication_value;
    uint32 entry_id_from_location;
    uint8 found;
    uint32 entry_id;
    dbal_tables_e dbal_table_id;
    uint32 fg_tcam_handler_id;
    uint32 entry_tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get max values from DNX data. */
    max_nof_tcam_entries = dnx_data_field.tcam.nof_entries_80_bits_get(unit);

    hit_indication_value = 0;
    *is_entry_found_p = FALSE;
    entry_id_from_location = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &fg_tcam_handler_id));

    for (entry_id = 0; entry_id < max_nof_tcam_entries; entry_id++)
    {
        /** Get the entry id from its location. */
        dnx_field_tcam_access_sw.entry_location_hash.get_by_index(unit, core_id, entry_id, &entry_id_from_location,
                                                                  &found);

        if (found)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_handler(unit, entry_id_from_location, &entry_tcam_handler_id));
            if (fg_tcam_handler_id == entry_tcam_handler_id)
            {
                uint32 hit_indication_value_uint32;
                SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_get
                                (unit, core_id, entry_id_from_location, &hit_indication_value));
                hit_indication_value_uint32 = hit_indication_value;
                if (utilex_bitstream_test_bit(&hit_indication_value_uint32, core_id))
                {
                    hit_indication_value = 1;
                    break;
                }
                else
                {
                    entry_id_from_location = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                    continue;
                }
            }
            else
            {
                entry_id_from_location = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                continue;
            }
        }
    }

    if (entry_id_from_location != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
    {
        last_diag_fg_info_p[fg_id_index].entry_id = entry_id_from_location;
        if (hit_indication_value)
        {
            *is_entry_found_p = TRUE;
        }
    }
    else
    {
        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function retrieve information about last hit DT entry per field group.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id_index - Iterator value to know, on which place to save the entry id.
 * \param [in] fg_id - Field group ID, which performed lookups and have a hit.
 * \param [out] last_diag_fg_info_p - Stores entry which was hit per FG.
 * \param [out] is_entry_found_p - Indication if we find at least one entry.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_dt_entry_info_get(
    int unit,
    bcm_core_t core_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *is_entry_found_p)
{
    dnx_field_entry_t entry_info;
    uint32 entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    *is_entry_found_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
    /**
     * Calculate the entry_handle, using the first array of the key_values, because in
     * case of DT we have 11 bit key and the key is the actual entry offset in the bank (location).
     */
    entry_handle = DNX_FIELD_ENTRY_TCAM_DT(last_diag_fg_info_p[fg_id_index].key_values[0][0], fg_id);
    SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }
    else
    {
        last_diag_fg_info_p[fg_id_index].entry_id = entry_handle;
        *is_entry_found_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *  This function displays master key raw value for External stage.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - The KBP opcode used.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_fg_master_key_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    uint8 master_key[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES],
    uint32 *master_key_len_p)
{
    uint32 entry_handle_id_fwd_context;
    uint32 entry_handle_id_acl_context;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key;
    uint32 aligner_key_size_single_fwd;
    uint32 aligner_key_size_single_acl;
    uint32 aligner_key_size_single;
    uint32 aligner_total_size;
    uint32 key_signal_size_uint8 = dnx_data_field.diag.key_signal_size_in_bytes_get(unit);
    uint32 key_signal_size_uint32 = BYTES2WORDS(key_signal_size_uint8);
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    uint32 fwd_context;
    uint32 acl_context;
    uint32 opcode_id;
    int nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(master_key, 0x0, sizeof(master_key[0]) * DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);

    /*
     * Get the fwd_context and acl_context.
     */
    opcode_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts(unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
    if (nof_fwd_contexts <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode %d has %d fwd contexts.\n", opcode_id, nof_fwd_contexts);
    }
    fwd_context = fwd_contexts[0];
    acl_context = acl_contexts[0];

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id_fwd_context));
    dbal_entry_key_field32_set(unit, entry_handle_id_fwd_context, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id_acl_context));
    dbal_entry_key_field32_set(unit, entry_handle_id_acl_context, DBAL_FIELD_CONTEXT_PROFILE, acl_context);

    aligner_total_size = 0;
    for (kbp_key = 0; kbp_key < nof_kbp_keys; kbp_key++)
    {
        char key_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        uint32 key_sig_value_uint32[BYTES2WORDS(DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES)] = { 0 };
        uint8 key_sig_value_uint8[DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES] = { 0 };
        uint8 key_sig_value_uint8_msb_to_lsb[DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES] = { 0 };
        int byte_index;

        dbal_entry_key_field32_set(unit, entry_handle_id_fwd_context, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_fwd_context, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_fwd_context, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE,
                         &aligner_key_size_single_fwd));

        dbal_entry_key_field32_set(unit, entry_handle_id_acl_context, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_acl_context, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_acl_context, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE,
                         &aligner_key_size_single_acl));

        aligner_key_size_single = aligner_key_size_single_fwd + aligner_key_size_single_acl;
        aligner_total_size += aligner_key_size_single;
        /** Sanity checks. */
        if (aligner_key_size_single > key_signal_size_uint8)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Aligner size of key %d is %d, larger than key size of %d.\n",
                         kbp_key, aligner_key_size_single, key_signal_size_uint8);
        }
        if (aligner_total_size > DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Total aligner size of key %d reached %d, larger than array size %d.\n",
                         kbp_key, aligner_total_size, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        }
        /** Get the key. */
        sal_snprintf(key_sig_name, sizeof(key_sig_name), "Key_%d", kbp_key);
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "KBP", key_sig_name, key_sig_value_uint32,
                                      key_signal_size_uint32));
        /** Convert to format of bytes from MSB to lsb. */
        SHR_IF_ERR_EXIT(utilex_U32_to_U8(key_sig_value_uint32, key_signal_size_uint8, key_sig_value_uint8));
        for (byte_index = 0; byte_index < key_signal_size_uint8; byte_index++)
        {
            key_sig_value_uint8_msb_to_lsb[byte_index] = key_sig_value_uint8[key_signal_size_uint8 - 1 - byte_index];
        }
        /** Copy key to master key. */
        sal_memcpy(&master_key[aligner_total_size - aligner_key_size_single], key_sig_value_uint8_msb_to_lsb,
                   aligner_key_size_single);
    }

    /** Sanity check: Check if the master key size from aligner fits the master key on opcode.*/
    {
        kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
        uint32 nof_segments;
        uint32 total_segment_size = 0;
        int segment_index;
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
        for (segment_index = 0; segment_index < nof_segments; segment_index++)
        {
            if (ms_key_segments[segment_index].is_overlay_field == FALSE)
            {
                total_segment_size += ms_key_segments[segment_index].nof_bytes;
            }
        }
        if (total_segment_size != aligner_total_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Total aligner size %d while master key size is %d.\n",
                         aligner_total_size, total_segment_size);
        }
    }

    (*master_key_len_p) = aligner_total_size;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *   This function retrieve information about last hit External entry per field group.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - The KBP opcode used.
 * \param [in] fg_id_index - Iterator value to know, on which place to save the entry id.
 * \param [in] fg_id - Field group ID, which performed lookups and have a hit.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [out] last_diag_fg_info_p - Stores entry which was hit per FG.
 * \param [out] is_entry_found_p - Indication if we find at least one entry.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_entry_info_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *is_entry_found_p)
{
    uint8 payload[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];
    uint32 payload_len;
    int qual_iter;
    int seg_index_on_master;
    int master_key_size;
    uint32 seg_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    dbal_tables_e dbal_table_id;
    int access_id, prio_len;
    struct kbp_entry *db_entry = NULL;
    uint32 key_id_iter;
    uint32 fwd2_context;
    uint32 last_key_value_array_offset;
    uint32 acl_context;
    bcm_field_AppType_t apptype;
    uint32 opcode_id;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint8 kbp_search_key[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    uint32 kbp_search_start_offset;
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    kbp_db_handles_t kbp_db_handles[DNX_DATA_MAX_ELK_GENERAL_NOF_DBS_PER_CORE];
    kbp_db_t_p db_p;
    dbal_core_mode_e core_mode;

    SHR_FUNC_INIT_VARS(unit);

    *is_entry_found_p = FALSE;
    last_key_value_array_offset = 0;
    kbp_search_start_offset = DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES;

    /** Get FWD and ACL contexts to retrieve the opcode_id. */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "IPMF1", "Context", &fwd2_context, 1));
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "IPMF1", "ACL_Context", &acl_context, 1));
    SHR_IF_ERR_EXIT(dnx_field_map_contexts_to_apptype_get(unit, fwd2_context, acl_context, &apptype));
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));
    /** Get all the segments on master key. */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));

    /** Find the indices of the corresponding segments to the qualifiers on the master key.*/
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_segment_indices(unit, fg_id, opcode_id,
                                                        seg_index_on_master_by_qual, qual_index_on_segment,
                                                        qual_offset_on_segment));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /** Get the master key value. */
    SHR_IF_ERR_EXIT(diag_dnx_field_last_external_fg_master_key_get(unit, core_id, context_id, payload, &payload_len));

    /** find the size of the master key.*/
    master_key_size = 0;
    for (seg_index_on_master = 0; seg_index_on_master < nof_segments; seg_index_on_master++)
    {
        master_key_size += BYTES2BITS(ms_key_segments[seg_index_on_master].nof_bytes);
    }

    /** Iterate over max number of qualifiers for FG and check if current qual is valid for it. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type != DNX_FIELD_QUAL_TYPE_INVALID;
         qual_iter++)
    {
        /** Location of the segment on the master key, from where the value will be read. */
        uint32 master_key_seg_offset = 0;
        uint32 seg_index;
        uint32 seg_size;

        seg_index = seg_index_on_master_by_qual[qual_iter];
        if (seg_index >= nof_segments)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "qualifier number %d in field group %d uses segment %d, only %d segments in master key.\r\n",
                         qual_iter, fg_id, seg_index, nof_segments);
        }

        if (ms_key_segments[seg_index].is_overlay_field)
        {
            if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id !=
                DNX_FIELD_FFC_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier 0x%x, FFC found, "
                             "but the qualifeir is an overlay segemnt.\n",
                             opcode_id, fg_id,
                             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
            }
        }
        else
        {
            if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.ffc_id ==
                DNX_FIELD_FFC_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, fg_id %d, qualifier 0x%x, no FFC found.\n",
                             opcode_id, fg_id,
                             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type);
            }
        }

        if (ms_key_segments[seg_index].is_overlay_field)
        {
            master_key_seg_offset = BYTES2BITS(ms_key_segments[seg_index].overlay_offset_bytes);
        }
        else
        {
            /** Iterate over segments to find the position of the segment on master key from MSB. */
            int seg_iter;
            uint32 seg_size_iter;
            for (seg_iter = 0; seg_iter < nof_segments; seg_iter++)
            {
                if (ms_key_segments[seg_iter].is_overlay_field)
                {
                    continue;
                }

                seg_size_iter = BYTES2BITS(ms_key_segments[seg_iter].nof_bytes);

                /**
                 * Compare the current segment name with the returned index
                 * of the current field qualifier on Master Key. In case yes,
                 * we found the correct place on the master key.
                 * Otherwise we increase the master_key_seg_offset with the current segment size.
                 */
                if (seg_iter == seg_index)
                {
                    break;
                }
                else
                {
                    master_key_seg_offset += seg_size_iter;
                }
            }
        }

        seg_size = BYTES2BITS(ms_key_segments[seg_index].nof_bytes);

        /** Store the offset on master key for every qualifier. */
        last_diag_fg_info_p[fg_id_index].kbp_info.quals_offset_on_master_key[qual_iter] =
            master_key_size - master_key_seg_offset - seg_size + qual_offset_on_segment[qual_iter];

        /*
         * Skip non first packed qualifers when building the key from the segments.
         */
        if (qual_index_on_segment[qual_iter] != 0)
        {
            continue;
        }

        /** Calculating the location for the current segment value in the kbp_search_key, which will be used for kbp_search. */
        kbp_search_start_offset -= BITS2BYTES(seg_size);
        sal_memcpy(&kbp_search_key[kbp_search_start_offset], &payload[BITS2BYTES(master_key_seg_offset)],
                   BITS2BYTES(seg_size));

        /** Constructing the kbp_acl_key_value, which will be used for presenting later in Key info table. */
        sal_memset(seg_val, 0, sizeof(seg_val));
        SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                        (unit, seg_size, &payload[BITS2BYTES(master_key_seg_offset)], seg_val));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (seg_val, last_key_value_array_offset, seg_size,
                         last_diag_fg_info_p[fg_id_index].kbp_info.kbp_acl_key_value));

        last_key_value_array_offset += seg_size;

        /** Store info for all used KBP keys by ACL. */
        for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_id_iter++)
        {
            if (last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] == DNX_FIELD_KEY_ID_INVALID)
            {
                last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] =
                    attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.key_id;
                break;
            }
            else if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[0].ffc.key_id !=
                     last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter])
            {
                continue;
            }
            else
            {
                break;
            }
        }
    }

    /** Sanity check */
    if (COUNTOF(kbp_db_handles) < DNX_KBP_NOF_DBS_PER_CORE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of elements in kbp_db_handles %d is less than %d.\n",
                     COUNTOF(kbp_db_handles), DNX_KBP_NOF_DBS_PER_CORE);
    }
    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                    (unit, dbal_table_id, DBAL_ACCESS_METHOD_KBP, (void *) &kbp_db_handles[0]));
    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
    if (core_mode == DBAL_CORE_MODE_SBC)
    {
        db_p = kbp_db_handles[0].db_p;
       /** Sanity check.*/
        if (kbp_db_handles[0].core_id != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected core ID %d for fg_id %d not entried_per_core.\n",
                         kbp_db_handles[0].core_id, fg_id);
        }
    }
    else if (core_mode == DBAL_CORE_MODE_DPC)
    {
        db_p = kbp_db_handles[core_id].db_p;
       /** Sanity check.*/
        if (kbp_db_handles[core_id].core_id != core_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected core ID %d for fg_id %d core_id %d entried_per_core.\n",
                         kbp_db_handles[0].core_id, fg_id, core_id);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected core mode %d for fg_id %d.\n", core_mode, fg_id);
    }

    /** Do kbp_db_search with the constructed kbp_search_key above. */
    DNX_KBP_TRY(kbp_db_search(db_p, &kbp_search_key[kbp_search_start_offset], &db_entry, &access_id, &prio_len));

    /**
     * In case of no match kbp_db_search returns NULL for db_entry and -1 for acces_id.
     * Otherwise store the retrieved entry ID in appropriate field format.
     */
    if (db_entry == NULL && access_id == -1)
    {
        *is_entry_found_p = FALSE;
    }
    else
    {
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            last_diag_fg_info_p[fg_id_index].entry_id =
                DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE(PTR_TO_INT(db_entry), core_id);
        }
        else
        {
            last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL(PTR_TO_INT(db_entry));
        }
        *is_entry_found_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

/**
 * \brief
 *   This function checks if current field group type
 *   is supported for last info diagnostic.
 * \param [in] unit - The unit number.
 * \param [in] fg_type - Field group type.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_is_fg_type_supported(
    int unit,
    dnx_field_group_type_e fg_type)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        case DNX_FIELD_GROUP_TYPE_CONST:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
        {
            /** In case of supported FG don't do anything, just exit. */
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        default:
        {
            char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            /** None of the supported types. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

            LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by Field Last Info diagnostic!! %s%s%s\r\n\n",
                       converted_fg_type, EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns action single size per field group type.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - The stage the field group in in.
 * \param [in] fg_type - Field group type.
 * \param [out] action_size_single_p - Action single size.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static void
diag_dnx_field_last_fg_type_action_single_size_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    uint32 *action_size_single_p)
{
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            *action_size_single_p = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *action_size_single_p = dnx_data_field.stage.stage_info_get(unit, field_stage)->exem_max_result_size;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *action_size_single_p = dnx_data_field.mdb_dt.action_size_large_get(unit);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            *action_size_single_p = dnx_data_field.tcam.action_size_half_get(unit);
            break;
        }
        default:
        {
            *action_size_single_p = -1;
            break;
        }
    }
}

/**
 * \brief
 *   This function returns last key DBAL table for adapter.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] fg_type - Field group type.
 * \param [in] converted_fg_type - String of field group type.
 * \param [out] last_key_dbal_table_p - Last key info DBAL table to be returned.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_type_last_key_dbal_table_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    char *converted_fg_type,
    dbal_tables_e * last_key_dbal_table_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                {
                    *last_key_dbal_table_p = DBAL_TABLE_FIELD_IPMF1_LAST_KEY_INFO;
                    break;
                }
                case DNX_FIELD_STAGE_IPMF2:
                {
                    *last_key_dbal_table_p = DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO;
                    break;
                }
                case DNX_FIELD_STAGE_IPMF3:
                {
                    *last_key_dbal_table_p = DBAL_TABLE_FIELD_IPMF3_LAST_KEY_INFO;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for %s FG on adapter (%s).\r\n",
                                 converted_fg_type, dnx_field_stage_text(unit, field_stage));
                }
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                *last_key_dbal_table_p = DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO;
            }
            else if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                *last_key_dbal_table_p = DBAL_TABLE_FIELD_IPMF3_LAST_KEY_INFO;
            }
            else if (field_stage == DNX_FIELD_STAGE_EPMF && dnx_data_field.features.dir_ext_epmf_get(unit))
            {
                *last_key_dbal_table_p = DBAL_TABLE_EMPTY;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for DE FG (%s).\r\n",
                             dnx_field_stage_text(unit, field_stage));
            }
            break;
        }
        default:
        {
            *last_key_dbal_table_p = DBAL_TABLE_EMPTY;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns Key, Result and Hit signal values.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] fg_type - Field group type.
 * \param [in] key_name - Key name as string.
 * \param [in] key_id - Key ID.
 * \param [in] names_p - Names of the blocks for signals.
 * \param [out] field_key_value_p - Key signal value.
 * \param [out] field_result_value_p - Result signal value.
 * \param [out] field_hit_value_p - Key hit signal value.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_type_last_key_res_hit_value_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    const char *key_name,
    dbal_enum_value_field_field_key_e key_id,
    diag_dnx_field_utils_sig_block_names_t * names_p,
    uint32 *field_key_value_p,
    uint32 *field_result_value_p,
    uint32 *field_hit_value_p)
{
    char key_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char result_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char hit_sig_name[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char *block_p = names_p->block;
    char *from_p = names_p->to_tcam;
    char *to_p = names_p->tcam;
    uint8 is_key_value_retrieved = FALSE;
    uint8 is_result_value_retrieved = FALSE;
    uint8 is_hit_value_retrieved = FALSE;
    uint32 key_signal_size;
    uint32 result_signal_size;
    uint32 hit_signal_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_snprintf(key_sig_name, sizeof(key_sig_name), "Key_%s", key_name);
    sal_snprintf(hit_sig_name, sizeof(hit_sig_name), "Hit_%s", key_name);
    sal_snprintf(result_sig_name, sizeof(result_sig_name), "Result_%s", key_name);

    key_signal_size = BYTES2WORDS(dnx_data_field.diag.key_signal_size_in_bytes_get(unit));
    result_signal_size = dnx_data_field.diag.result_signal_size_in_words_get(unit);
    hit_signal_size = dnx_data_field.diag.hit_signal_size_in_words_get(unit);

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            /** We have to read different signals per stage. */
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                {
                    to_p = "LEXEM1";
                    sal_strncpy_s(hit_sig_name, "Hit", sizeof(hit_sig_name));
                    sal_strncpy_s(result_sig_name, "Result", sizeof(result_sig_name));
                    break;
                }
                case DNX_FIELD_STAGE_IPMF2:
                {
                    SHR_IF_ERR_EXIT(dpp_dsig_read
                                    (unit, core_id, block_p, from_p, "SEXEM3orDE", "Key_J", field_key_value_p,
                                     key_signal_size));

                    is_key_value_retrieved = TRUE;
                    to_p = "SEXEM3";
                    sal_strncpy_s(hit_sig_name, "Hit", sizeof(hit_sig_name));
                    sal_strncpy_s(result_sig_name, "Result", sizeof(result_sig_name));
                    break;
                }
                case DNX_FIELD_STAGE_IPMF3:
                {
                    to_p = "SEXEM3";
                    sal_strncpy_s(hit_sig_name, "Hit", sizeof(hit_sig_name));
                    sal_strncpy_s(result_sig_name, "Result", sizeof(result_sig_name));
                    break;
                }
                case DNX_FIELD_STAGE_EPMF:
                {
                    to_p = "LEXEM1";
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%s).\r\n", dnx_field_stage_text(unit, field_stage));
                }
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /** We have to read different signals per key. */
            switch (key_id)
            {
                case DBAL_ENUM_FVAL_FIELD_KEY_C:
                {
                    if (field_stage == DNX_FIELD_STAGE_IPMF3)
                    {
                        to_p = "SEXEM3";
                    }
                    else
                    {
                        to_p = "TCAMorDT";
                    }
                    break;
                }
                case DBAL_ENUM_FVAL_FIELD_KEY_H:
                {
                    to_p = "TCAMorDT";
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key (%s).\r\n", key_name);
                }
            }
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, core_id, block_p, "DT", from_p, "Hit", field_hit_value_p, hit_signal_size));
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, core_id, block_p, "DT", from_p, "Result", field_result_value_p,
                             dnx_data_field.diag.dt_result_signal_size_in_words_get(unit)));

            is_hit_value_retrieved = TRUE;
            is_result_value_retrieved = TRUE;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                to_p = "TCAMorDE";
                SHR_IF_ERR_EXIT(dpp_dsig_read
                                (unit, core_id, block_p, "TCAM", from_p, hit_sig_name, field_hit_value_p,
                                 hit_signal_size));
                SHR_IF_ERR_EXIT(dpp_dsig_read
                                (unit, core_id, block_p, "TCAM", from_p, result_sig_name, field_result_value_p,
                                 dnx_data_field.diag.dt_result_signal_size_in_words_get(unit)));

                is_hit_value_retrieved = TRUE;
                is_result_value_retrieved = TRUE;
            }
            else
            {
                to_p = "TCAM";
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            is_key_value_retrieved = TRUE;
            is_hit_value_retrieved = TRUE;
            is_result_value_retrieved = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    if (is_key_value_retrieved == FALSE)
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, block_p, from_p, to_p, key_sig_name, field_key_value_p, key_signal_size));
    }
    if (is_hit_value_retrieved == FALSE)
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, block_p, to_p, from_p, hit_sig_name, field_hit_value_p, hit_signal_size));
    }
    if (is_result_value_retrieved == FALSE)
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, block_p, to_p, from_p, result_sig_name, field_result_value_p,
                         result_signal_size));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets key_values and key_ids arrays in last_diag_fg_info_p.
 * \param [in] unit - The unit number.
 * \param [in] key_id_iter - Index to access key_values and key_ids array in last_diag_fg_info_p.
 * \param [in] fg_id_index - Index to access arrays in fg_performed_lookup_p.
 * \param [in] fg_type - Field group type.
 * \param [in] key_id - Key ID.
 * \param [in] field_key_value_p - Key signal value.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static void
diag_dnx_field_last_fg_type_fg_info_key_values_ids_set(
    int unit,
    int key_id_iter,
    int fg_id_index,
    dnx_field_group_type_e fg_type,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *field_key_value_p,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p)
{
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] = key_id;
            sal_memcpy(last_diag_fg_info_p[fg_id_index].key_values[key_id_iter], field_key_value_p,
                       (sizeof(uint32) * DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY));
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * \brief
 *   This function sets result_values array in last_diag_fg_info_p.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - PMF stage.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id_index - Index to access arrays in fg_performed_lookup_p.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_type - Field group type.
 * \param [in] action_size_single - Payload size in case of single key size.
 * \param [in] result_value_start_offset_p - Sum of action_size_single sizes in case
 *  of double key field groups.
 * \param [in] field_result_value_p - Payload values from the signals.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_type_fg_info_res_values_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e fg_type,
    uint32 action_size_single,
    int *result_value_start_offset_p,
    uint32 *field_result_value_p,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p)
{
    int key_select_lsb_zero_padding;
    uint32 field_result_value_shifted[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Find if the result on the payload signal is shifted compared to the field IO.
     */
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            if (field_stage == DNX_FIELD_STAGE_EPMF
                && dnx_data_field.features.epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get(unit))
            {
                key_select_lsb_zero_padding = 0;
            }
            else
            {
                dbal_enum_value_field_field_io_e field_io;
                unsigned int action_input_select;
                unsigned int required_shift;
                int found;
                int fes_key_select_resolution_in_bits;

                SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id, &field_io));
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_key_select_get
                                (unit, field_io, field_stage, 1, 0, FALSE, &action_input_select, &required_shift,
                                 &found));
                if (found == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to find key select for first bit on field_io. "
                                 "field_io %d, stage %d, cotext %d fg_id %d fg_type %d.\r\n",
                                 field_io, field_stage, context_id, fg_id, fg_type);
                }
                fes_key_select_resolution_in_bits =
                    dnx_data_field.stage.stage_info_get(unit, field_stage)->fes_key_select_resolution_in_bits;
                if (fes_key_select_resolution_in_bits <= 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "fes_key_select_resolution_in_bits is %d. "
                                 "stage %d, cotext %d fg_id %d fg_type %d.\r\n",
                                 fes_key_select_resolution_in_bits, field_stage, context_id, fg_id, fg_type);
                }
                key_select_lsb_zero_padding = required_shift % fes_key_select_resolution_in_bits;
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            /*
             * Get all 64b for TCAM-DT in order to reach MSB-part 
             */
            action_size_single = 64;
            /*
             * Fall- through 
             */
        }
        default:
        {
            key_select_lsb_zero_padding = 0;
            break;
        }
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (field_result_value_p, key_select_lsb_zero_padding,
                     BYTES2BITS(sizeof(field_result_value_shifted)) - key_select_lsb_zero_padding,
                     field_result_value_shifted));

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (field_result_value_shifted, *result_value_start_offset_p,
                             action_size_single, last_diag_fg_info_p[fg_id_index].result_values));
            *result_value_start_offset_p += action_size_single;
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets 'Hit' and 'Entry Id' cells for 'Field Group Info' table.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id_index - Index to access arrays in fg_performed_lookup_p.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_type - Field group type.
 * \param [in] one_pkt - if the user has committed that only one packet was injected, additional info will be print.
 * \param [in] field_key_hit - Key hit signal value.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_type_entry_info_cells_print(
    int unit,
    bcm_core_t core_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e fg_type,
    uint8 one_pkt,
    uint32 field_key_hit,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    prt_control_t * prt_ctr)
{
    uint8 is_entry_found_p = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            if (DIAG_DNX_FIELD_LAST_IS_SIM)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_tcam_entry_info_get
                                (unit, core_id, fg_id_index, fg_id, last_diag_fg_info_p, &is_entry_found_p));

                /**
                 * In case of adapter we rely on returned hit indication 'is_entry_found_p' by
                 * diag_dnx_field_last_tcam_entry_info_get(), because there are no mapped 'Hit' signals .
                 */
                if (is_entry_found_p)
                {
                    /** Set 'Hit' cell. */
                    PRT_CELL_SET("%s", "Yes");
                    /** Set 'Entry Id' cell. */
                    PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                }
                else
                {
                    /** Set 'Hit' cell. */
                    PRT_CELL_SET("%s", "No");
                    last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                    /** Set 'Entry Id' cell. */
                    PRT_CELL_SET("%s", "No Entries");
                }
            }
            else
            {
                if (field_key_hit)
                {
                    /** Set 'Hit' cell. */
                    PRT_CELL_SET("%s", "Yes");
                    last_diag_fg_info_p[fg_id_index].entry_hit = TRUE;
                    /** Set 'Entry Id' cell. */
                    if (one_pkt == TRUE)
                    {
                        SHR_IF_ERR_EXIT(diag_dnx_field_last_tcam_entry_info_get
                                        (unit, core_id, fg_id_index, fg_id, last_diag_fg_info_p, &is_entry_found_p));

                        PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                    }
                    else
                    {
                        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                        PRT_CELL_SET("%s", "N/A");
                    }
                }
                else
                {
                    /** Set 'Hit' cell. */
                    PRT_CELL_SET("%s", "No");
                    last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                    /** Set 'Entry Id' cell. */
                    PRT_CELL_SET("%s", "No Entries");
                }
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            /** In case of DE, there is not meaning of HIT, because of that print N/A. */
            /** Set 'Hit' cell. */
            PRT_CELL_SET("%s", "N/A");

            last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
            /** Set 'Entry Id' cell. */
            PRT_CELL_SET("%s", "N/A");
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            /**
             * In case of hit we should present 'Yes' in the relevant sell,
             * and to update the output arrays.
             */
            if (field_key_hit)
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "Yes");
                last_diag_fg_info_p[fg_id_index].entry_hit = TRUE;
                /**
                 * In case of HIT, set entry ID to be different than
                 * DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1), it will be checked in next functions.
                 */
                last_diag_fg_info_p[fg_id_index].entry_id = 0;
            }
            else
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "No");
                /**
                 * In case no HIT, set entry ID to DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1),
                 * it will be checked in next functions.
                 */
                last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
            }

            /** Set 'Entry Id' cell. */
            PRT_CELL_SET("%s", "N/A");
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /**
             * In case of hit we should present 'Yes' in the relevant sell,
             * and to update the output arrays.
             */
            if (field_key_hit)
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "Yes");
                last_diag_fg_info_p[fg_id_index].entry_hit = TRUE;
                /**
                 * In case of HIT, set entry ID to be different than
                 * DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1), it will be checked in next functions.
                 */
                last_diag_fg_info_p[fg_id_index].entry_id = 0;
            }
            else
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "No");
                /**
                 * In case no HIT, set entry ID to DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1),
                 * it will be checked in next functions.
                 */
                last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_dt_entry_info_get
                            (unit, core_id, fg_id_index, fg_id, last_diag_fg_info_p, &is_entry_found_p));

            if (!is_entry_found_p)
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "No");
                /** Set 'Entry Id' cell. */
                PRT_CELL_SET("%s", "No Entries");
            }
            else
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "Yes");
                /** Set 'Entry Id' cell. */
                PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                last_diag_fg_info_p[fg_id_index].entry_hit = TRUE;
            }
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all relevant cells for 'Field Group Info' table.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] one_pkt - if the user commited that only one packet was injected, additional info will be print.
 * \param [in] visibility_mode - Visibility mode.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id_index - Index to access arrays in fg_performed_lookup_p.
 * \param [in] fg_id - Field group ID.
 * \param [in] names_p - Names of the blocks for signals
 * \param [out] fg_performed_lookup_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [out] num_fgs - Number of the field groups, which performed lookups.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 * \param [out] kbp_hitbit_mismatch_p - Compare result between retrieved hit signal value and searched result.
 *  If TRUE then we have mismatch otherwise FALSE.
 * \param [out] do_display_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_print_cells_set(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    uint8 one_pkt,
    bcm_instru_vis_mode_control_t visibility_mode,
    dnx_field_context_t context_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    diag_dnx_field_utils_sig_block_names_t * names_p,
    dnx_field_group_t * fg_performed_lookup_p,
    int *num_fgs,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *kbp_hitbit_mismatch_p,
    int *do_display_p,
    prt_control_t * prt_ctr)
{
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    int key_id_iter;
    field_group_name_t group_name;
    uint8 add_new_fg_id;
    uint32 field_key_hit;
    dnx_field_group_type_e fg_type;
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    uint8 is_entry_found_p;
#endif
    /**
     * An array of 5 elements to store 160 bits
     * key information, from the signals.
     */
    uint32 field_key_value[BYTES2WORDS(DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES)] = { 0 };
    /**
     * An array of 4 elements to store 128 bits
     * result information, from the signals.
     */
    uint32 field_result_value[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)] = { 0 };
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 action_size_single;
    int result_value_start_offset = 0;
    dbal_tables_e last_key_dbal_table = DBAL_TABLE_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    field_key_hit = 0;
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    is_entry_found_p = FALSE;
#endif
    action_size_single = 0;
    sal_memset(field_result_value, 0, sizeof(field_result_value));
    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(diag_dnx_field_last_is_fg_type_supported(unit, fg_type));
    diag_dnx_field_last_fg_type_action_single_size_get(unit, field_stage, fg_type, &action_size_single);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    /** Take the name for current FG from the SW state. */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_id, &group_name));
    /**
     * Check if Field Group name is being provided, in case no set N/A as default value,
     * in other case set the provided name.
     */
    /** Set 'FG Id/Name' cell. */
    if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
    {
        PRT_CELL_SET("%d/\"%s\"", fg_id, group_name.value);
    }
    else
    {
        PRT_CELL_SET("%d/\"%s\"", fg_id, "N/A");
    }

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
    /** Set 'FG Type' cell. */
    PRT_CELL_SET("%s", converted_fg_type);

    if (DIAG_DNX_FIELD_LAST_IS_SIM || fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_type_last_key_dbal_table_get
                        (unit, field_stage, fg_type, converted_fg_type, &last_key_dbal_table));
    }

    /** Get attach info for the current context in fgs_per_context[]. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    /**
     * On every new iteration of FG IDs we should add new line with underscore separator.
     * For this case we are using following flag add_new_fg_id, which is being set to TRUE for this purpose.
     */
    add_new_fg_id = TRUE;

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /** Iterate over max number of keys in double key and check if we a valid key id. */
            for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
                 attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
            {
                /** In case the flag is not set, we should skip the first cell related to FG ID. */
                if (!add_new_fg_id)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /** Skip 'FG Id/Name', 'FG Type' cells. */
                    PRT_CELL_SKIP(2);
                }

                /** Set 'Key Id' cell. */
                PRT_CELL_SET("%s",
                             dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FIELD_KEY,
                                                         attach_full_info->key_id.id[key_id_iter]));

                if ((DIAG_DNX_FIELD_LAST_IS_SIM || fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION) &&
                    (last_key_dbal_table != DBAL_TABLE_EMPTY))
                {
                    
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_last_key_table_info_get
                                    (unit, last_key_dbal_table, core_id,
                                     attach_full_info->key_id.id[key_id_iter], field_key_value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_type_last_key_res_hit_value_get
                                    (unit, core_id, field_stage, fg_type,
                                     dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FIELD_KEY,
                                                                 attach_full_info->key_id.id[key_id_iter]),
                                     attach_full_info->key_id.id[key_id_iter], names_p, field_key_value,
                                     field_result_value, &field_key_hit));
                }

                /** Set 'Raw Data' cell. */
                if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION &&
                    one_pkt == FALSE && visibility_mode != bcmInstruVisModeAlways)
                {
                    /** In case of DE, if vis mode is not Always, no need to present key just N/A. */
                    PRT_CELL_SET("%s", "N/A");
                }
                else if ((fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB) && (field_stage == DNX_FIELD_STAGE_IPMF3))
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                else
                {
                    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);
                }

                diag_dnx_field_last_fg_type_fg_info_key_values_ids_set(unit, key_id_iter, fg_id_index, fg_type,
                                                                       attach_full_info->key_id.id[key_id_iter],
                                                                       field_key_value, last_diag_fg_info_p);

                SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_type_fg_info_res_values_set
                                (unit, field_stage, context_id, fg_id_index, fg_id, fg_type, action_size_single,
                                 &result_value_start_offset, field_result_value, last_diag_fg_info_p));

                /**
                 * In case the flag is set, it means that a new field group ID was added to the relevant column
                 * Then we should set value for Entry ID on this roll for this FG and after that to set
                 * the flag to FALSE.
                 */
                if (add_new_fg_id)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_type_entry_info_cells_print(unit, core_id, fg_id_index,
                                                                                       fg_id, fg_type, one_pkt,
                                                                                       field_key_hit,
                                                                                       last_diag_fg_info_p, prt_ctr));

                    /**
                     * Set the flag to FALSE, if we have more then one KEY for current FG,
                     * we should skip the cells related to FG ID and Entry ID.
                     */
                    add_new_fg_id = FALSE;
                }
                else
                {
                    /** Skip 'Hit', 'Entry Id' cells. */
                    PRT_CELL_SKIP(2);
                }

                *do_display_p = TRUE;
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            /** Set 'Key Id' cell. */
            PRT_CELL_SET("%s", "N/A");
            /** Set 'Raw Data' cell. */
            PRT_CELL_SET("%s", "N/A");
            /** Set 'Hit' cell. */
            PRT_CELL_SET("%s", "N/A");
            /** Set 'Entry Id' cell. */
            PRT_CELL_SET("%s", "N/A");

            *do_display_p = TRUE;
            break;
        }
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            char *block_p = names_p->block;
            char *from_p = names_p->to_tcam;
            char *to_p = names_p->tcam;
            uint32 hit_bit_buff = 0;
            uint32 rpf_error = 0;
            uint32 fwd_error = 0;

            SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, block_p, "KBP", from_p, "RPF_Error", &rpf_error, 1));
            SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, block_p, "KBP", from_p, "Fwd_Error", &fwd_error, 1));

            /** Check if there is an error in External stage. */
            if ((rpf_error == TRUE) || (fwd_error == TRUE))
            {
                LOG_CLI_EX("\r\n" "Note: There is an External Error RPF_Error = %d | FWD_Error = %d !!!%s%s\r\n\n",
                           rpf_error, fwd_error, EMPTY, EMPTY);
            }

            /** Set Key ID and Raw Data columns. */
            PRT_CELL_SET("%s", "N/A");
            PRT_CELL_SET("%s", "N/A");

            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, core_id, block_p, from_p, to_p, "Elk_Lkp_Payload.HitBit", &hit_bit_buff,
                             BYTES2WORDS(dnx_data_field.diag.key_signal_size_in_bytes_get(unit))));

            /**
             * Get the value of the hit bit from the 8b buffer signal value.
             * For payload_id 0 the bit is located in the msb part of the buffer.
             */
            field_key_hit = utilex_bitstream_test_bit(&hit_bit_buff,
                                                      ((DNX_KBP_HIT_INDICATION_SIZE_IN_BITS - 1) -
                                                       attach_full_info->attach_basic_info.payload_id));

            SHR_IF_ERR_EXIT(diag_dnx_field_last_external_entry_info_get
                            (unit, core_id, context_id, fg_id_index, fg_id, attach_full_info, last_diag_fg_info_p,
                             &is_entry_found_p));

            /**
             * In case of hit we should present 'Yes' in the relevant sell,
             * and to update the output arrays.
             */
            if (field_key_hit && is_entry_found_p)
            {
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "Yes");
                /** Set 'Entry Id' cell. */
                PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                last_diag_fg_info_p[fg_id_index].entry_hit = TRUE;
            }
            else
            {
                last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                /** Set 'Hit' cell. */
                PRT_CELL_SET("%s", "No");
                /** Set 'Entry Id' cell. */
                PRT_CELL_SET("%s", "No Entries");
            }

            if (field_key_hit != is_entry_found_p)
            {
                *kbp_hitbit_mismatch_p = TRUE;
                LOG_CLI_EX
                    ("Mismatch between hit bit signal value %d and search result value %d for FG ID %d!\r\n %s",
                     field_key_hit, is_entry_found_p, fg_id, EMPTY);
            }

            *do_display_p = TRUE;
#else /* defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Group Type External should not exit, "
                         "as KBP is not compiled, and yet it does!!\r\n");
#endif /* defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
            break;
        }
#endif
        default:
        {
            break;
        }
    }

    if (*do_display_p)
    {
        fg_performed_lookup_p[fg_id_index] = fg_id;
        (*num_fgs)++;
    }

exit:
    SHR_FREE(attach_full_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about which FGs performed lookups and
 *   the keys which they are using.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] one_pkt - if the user commited that only one packet was injected, additional info will be print.
 * \param [in] visibility_mode - Visibility mode.
 * \param [in] context_id - ID of the selected context.
 * \param [in] names_p - Names of the blocks for signals
 * \param [out] fg_performed_lookup_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [out] num_fgs - Number of the field groups, which performed lookups.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 * \param [out] kbp_hitbit_mismatch_p - Compare result between retrieved hit signal value and searched result.
 *  If TRUE then we have mismatch otherwise FALSE.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    uint8 one_pkt,
    bcm_instru_vis_mode_control_t visibility_mode,
    dnx_field_context_t context_id,
    diag_dnx_field_utils_sig_block_names_t * names_p,
    dnx_field_group_t * fg_performed_lookup_p,
    int *num_fgs,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *kbp_hitbit_mismatch_p,
    sh_sand_control_t * sand_control)
{
    int fg_id_index;
    int nof_fgs_per_context;
    dnx_field_group_t fgs_per_context[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    int do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    do_display = FALSE;
    nof_fgs_per_context = 0;

    if (one_pkt == FALSE && visibility_mode != bcmInstruVisModeAlways && field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        LOG_CLI_EX("\r\n" "FG info for IPMF3 stage can be presented only if visibility mode is Always!!!%s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get
                        (unit, field_stage, context_id, fgs_per_context, &nof_fgs_per_context));
    }

    if (!nof_fgs_per_context)
    {
        if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            bcm_field_AppType_t apptype;
            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, context_id, &apptype));
            LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d (AT) !!!%s%s%s\r\n\n", apptype, EMPTY,
                       EMPTY, EMPTY);
        }
        else
        {
            LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d !!!%s%s%s\r\n\n", context_id, EMPTY,
                       EMPTY, EMPTY);
        }
        SHR_EXIT();
    }

    PRT_TITLE_SET("Field Group Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Id/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Data");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Hit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Id");

    /** Print all used keys by the attached FGs. */
    for (fg_id_index = 0; fg_id_index < nof_fgs_per_context; fg_id_index++)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print_cells_set(unit, core_id, field_stage, one_pkt, visibility_mode,
                                                               context_id, fg_id_index, fgs_per_context[fg_id_index],
                                                               names_p, fg_performed_lookup_p, num_fgs,
                                                               last_diag_fg_info_p, kbp_hitbit_mismatch_p, &do_display,
                                                               prt_ctr));
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
        LOG_CLI_EX("\r\n" "No info for FGs, to be presented!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers of a specific Entry.
 *   Used for TCAM FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID.
 * \param [in] context_id - ID of the selected context.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_qual_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    uint8 is_hit_found,
    int qual_iter,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int qual_val_mask_iter, val_iter;
    int max_val_iter;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_exp_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_mask_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 qual_lsb;
    uint32 single_key_size;
    int qual_valid;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = BITS2WORDS(fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);

    /** Sanity check */
    if (max_val_iter > COUNTOF(qual_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "max_val_iter is %d, maximum is %d. qual size is %d.\n",
                     max_val_iter, DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY,
                     fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    }

    sal_strncpy_s(qual_exp_val_string, "0x", sizeof(qual_exp_val_string));
    sal_strncpy_s(qual_mask_string, "0x", sizeof(qual_mask_string));
    for (qual_val_mask_iter = max_val_iter - 1; qual_val_mask_iter >= 0; qual_val_mask_iter--)
    {
        char qual_val_mas_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.
                                              qual_info[qual_iter].qual_value[qual_val_mask_iter]);
        sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_exp_val_string, "%s", qual_val_mas_buff);
        sal_strncat_s(qual_exp_val_string, qual_val_mas_buff, sizeof(qual_exp_val_string));

        sal_strncpy_s(qual_val_mas_buff, "", sizeof(qual_val_mas_buff));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.
                                              qual_info[qual_iter].qual_mask[qual_val_mask_iter]);
        sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_mask[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_mask_string, "%s", qual_val_mas_buff);
        sal_strncat_s(qual_mask_string, qual_val_mas_buff, sizeof(qual_mask_string));
    }

    /** Check if the current qual is valid for the entry. */
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        qual_valid = utilex_bitstream_have_one_in_range(entry_info_p->key_info.qual_info[qual_iter].qual_value,
                                                        0,
                                                        WORDS2BITS(DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY) -
                                                        1);
    }
    else
    {
        qual_valid = utilex_bitstream_have_one_in_range(entry_info_p->key_info.qual_info[qual_iter].qual_mask,
                                                        0,
                                                        WORDS2BITS(DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY) -
                                                        1);
    }

    /** Print all needed info about current qualifier. */
    /** Set 'Qual Type' cell. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    qual_lsb = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb +
        fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key;
    if (qual_lsb >= single_key_size)
    {
        qual_lsb -= single_key_size;
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p, qual_lsb,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy_s(qual_val_string, "0x", sizeof(qual_val_string));
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat_s(qual_val_string, val_buff, sizeof(qual_val_string));
    }

    /** Set 'Packet Value' cell. */
    PRT_CELL_SET("%s", qual_val_string);
    if (qual_valid)
    {
        if (is_hit_found)
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", qual_exp_val_string);
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                /** Set 'Entry Key Mask' cell. */
                PRT_CELL_SET("%s", qual_mask_string);
            }
        }
        else
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "N/A");
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                /** Set 'Entry Key Mask' cell. */
                PRT_CELL_SET("%s", "N/A");
            }
        }
    }
    else
    {
        if (is_hit_found)
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "-");
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                /** Set 'Entry Key Mask' cell. */
                PRT_CELL_SET("%s", "-");
            }
        }
        else
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "N/A");
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                /** Set 'Entry Key Mask' cell. */
                PRT_CELL_SET("%s", "N/A");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers by DE FGs.
 * \param [in] unit - The unit number.
 * \param [in] context_id - ID of the selected context.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] qual_size - Size of the current qualifier.
 * \param [in] qual_lsb - Lsb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different.
 * \param [in] qual_msb - Msb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_qual_print(
    int unit,
    dnx_field_context_t context_id,
    int qual_iter,
    uint8 qual_size,
    uint8 qual_lsb,
    uint8 qual_msb,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    int val_iter;
    int max_val_iter;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 qual_lsb_internal;
    uint32 single_key_size;
    int qual_additional_offset;

    SHR_FUNC_INIT_VARS(unit);

    qual_additional_offset = 0;

    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = BITS2WORDS(fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);

    /** Sanity check */
    if (max_val_iter > COUNTOF(qual_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "max_val_iter is %d, maximum is %d. qual size is %d.\n",
                     max_val_iter, DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY,
                     fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    }

    /** Set current qualifier info. */
    /** Set 'Qual Type' cell. */
    if (qual_lsb != 0 || qual_msb != 0)
    {
        PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type), qual_lsb,
                     qual_msb);

        qual_additional_offset = qual_lsb;
    }
    else
    {
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    }

    /** Set 'Qual LSB' cell. */
    PRT_CELL_SET("%d", (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb + qual_additional_offset));
    /** Set 'Qual Size' cell. */
    PRT_CELL_SET("%d", qual_size);

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    qual_lsb_internal = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb +
        fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key;
    if (qual_lsb_internal >= single_key_size)
    {
        qual_lsb_internal -= single_key_size;
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p, qual_lsb_internal,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy_s(qual_val_string, "0x", sizeof(qual_val_string));
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat_s(qual_val_string, val_buff, sizeof(qual_val_string));
    }

    /** Set 'Packet Value' cell. */
    PRT_CELL_SET("%s", qual_val_string);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers of a specific Entry.
 *   Used for EXEM FG.
 * \param [in] unit - The unit number.
 * \param [in] context_id - ID of the selected context.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_exem_qual_print(
    int unit,
    dnx_field_context_t context_id,
    uint8 is_hit_found,
    int qual_iter,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int qual_val_iter, val_iter;
    int max_val_iter;
    int qual_lsb;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_exp_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = BITS2WORDS(fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);

    /** Sanity check */
    if (max_val_iter > COUNTOF(qual_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "max_val_iter is %d, maximum is %d. qual size is %d.\n",
                     max_val_iter, DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY,
                     fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    }

    sal_strncpy_s(qual_exp_val_string, "0x", sizeof(qual_exp_val_string));
    for (qual_val_iter = max_val_iter - 1; qual_val_iter >= 0; qual_val_iter--)
    {
        char qual_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_iter]);
        sal_snprintf(qual_val_buff, sizeof(qual_val_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_exp_val_string, "%s", qual_val_buff);
        sal_strncat_s(qual_exp_val_string, qual_val_buff, sizeof(qual_exp_val_string));
    }

    /** Print all needed info about current qualifier. */
    /** Set 'Qual Type' cell. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    qual_lsb = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb +
        fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                   qual_lsb,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy_s(qual_val_string, "0x", sizeof(qual_val_string));
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat_s(qual_val_string, val_buff, sizeof(qual_val_string));
    }

    if ((fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB) &&
        (fg_info_p->group_basic_info.field_stage == DNX_FIELD_STAGE_IPMF3))
    {
        PRT_CELL_SET("%s", "N/A");
        PRT_CELL_SET("%s", "N/A");
    }
    else
    {
        /** Set 'Packet Value' cell. */
        PRT_CELL_SET("%s", qual_val_string);
        /** Set 'Entry Key Value' cell. */
        if (is_hit_found)
        {
            PRT_CELL_SET("%s", qual_exp_val_string);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *   This function sets all needed information about used qualifiers of a specific Entry.
 *   Used for External FG.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] qual_lsb_on_master_key - Lsb, of the current qualifier, on master key.
 * \param [in] qual_lsb_on_current_key - Lsb in key_values_p array, from where the
 *  value of current qualifier should be read.
 * \param [in] qual_size - Size of the current qualifier.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_qual_print(
    int unit,
    uint8 is_hit_found,
    uint32 qual_lsb_on_master_key,
    uint32 qual_lsb_on_current_key,
    uint32 qual_size,
    int qual_iter,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int qual_val_mask_iter, val_iter;
    int max_val_iter;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_exp_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_mask_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int qual_valid;
    int ent_qual_iter;

    SHR_FUNC_INIT_VARS(unit);

    qual_valid = FALSE;

    /** Iterate over entry actions array and check, which qualifiers are valid. */
    for (ent_qual_iter = 0; ent_qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         entry_info_p->key_info.qual_info[ent_qual_iter].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; ent_qual_iter++)
    {
        if (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type ==
            entry_info_p->key_info.qual_info[ent_qual_iter].dnx_qual)
        {
            qual_valid = TRUE;
            break;
        }
    }

    /** Set the right, max value to iterate on, depends on current qual_size. */
    max_val_iter = BITS2WORDS(qual_size);

    if (qual_valid)
    {
        sal_strncpy_s(qual_exp_val_string, "0x", sizeof(qual_exp_val_string));
        sal_strncpy_s(qual_mask_string, "0x", sizeof(qual_mask_string));
        for (qual_val_mask_iter = max_val_iter - 1; qual_val_mask_iter >= 0; qual_val_mask_iter--)
        {
            char qual_val_mas_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%08X",
                                                  entry_info_p->key_info.
                                                  qual_info[ent_qual_iter].qual_value[qual_val_mask_iter]);
            sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                         entry_info_p->key_info.qual_info[ent_qual_iter].qual_value[qual_val_mask_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_exp_val_string, "%s", qual_val_mas_buff);
            sal_strncat_s(qual_exp_val_string, qual_val_mas_buff, sizeof(qual_exp_val_string));

            sal_strncpy_s(qual_val_mas_buff, "", sizeof(qual_val_mas_buff));
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%08X",
                                                  entry_info_p->key_info.
                                                  qual_info[ent_qual_iter].qual_mask[qual_val_mask_iter]);
            sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                         entry_info_p->key_info.qual_info[ent_qual_iter].qual_mask[qual_val_mask_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_mask_string, "%s", qual_val_mas_buff);
            sal_strncat_s(qual_mask_string, qual_val_mas_buff, sizeof(qual_mask_string));
        }
    }

    /** Print all needed info about current qualifier. */
    /** Set 'Qual Type' cell. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    /** Set 'Qual LSB on master key' cell. */
    PRT_CELL_SET("%d", qual_lsb_on_master_key);

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p, qual_lsb_on_current_key, qual_size, qual_val));

    sal_strncpy_s(qual_val_string, "0x", sizeof(qual_val_string));
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat_s(qual_val_string, val_buff, sizeof(qual_val_string));
    }

    /** Set 'Packet Value' cell. */
    PRT_CELL_SET("%s", qual_val_string);
    if (qual_valid)
    {
        if (is_hit_found)
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", qual_exp_val_string);
            /** Set 'Entry Key Mask' cell. */
            PRT_CELL_SET("%s", qual_mask_string);
        }
        else
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "N/A");
            /** Set 'Entry Key Mask' cell. */
            PRT_CELL_SET("%s", "N/A");
        }
    }
    else
    {
        if (is_hit_found)
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "-");
            /** Set 'Entry Key Mask' cell. */
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            /** Set 'Entry Key Value' cell. */
            PRT_CELL_SET("%s", "N/A");
            /** Set 'Entry Key Mask' cell. */
            PRT_CELL_SET("%s", "N/A");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

/**
 * \brief
 *   This function sets all cells with information about performed FES actions of a specific Entry.
 *   Used for TCAM FGs.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] action_iter - Number of the current action in field group.
 * \param [in] fg_id - Field group ID.
 * \param [in] context_id - Context ID
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] result_values_p - Values of the results returned by signals.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in,out] action_non_void_iter_p - Number of the current action in the field groups amongst non void actions.
 * \param [out] start_bit_p - In-out parameter, which is used to store the offset where the
 *  next action value should be written to action_result_buff_p.
 * \param [out] action_result_buff_p - The action result buffer, which is used to store
 *  the values for all FES actions.
 * \param [out] mismatch_act_error_print_p - Indicates if there is any mismatch found during
 *  the actions printing.
 * \param [out] action_ace_entry_id_value_p - In case current entry contains bcmFieldActionAceEntryId action
 *  the value of it will be stored in action_ace_entry_id_value_p and used to present ACE Info table.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_action_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    int action_iter,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 *result_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    dnx_field_entry_t * entry_info_p,
    int *action_non_void_iter_p,
    int *start_bit_p,
    uint32 *action_result_buff_p,
    uint8 *mismatch_act_error_print_p,
    uint32 *action_ace_entry_id_value_p,
    prt_control_t * prt_ctr)
{
    unsigned int action_size;
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    char action_val_exp_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int ent_action_iter;
    int action_valid;
    uint32 action_tcam_result_val;
    int action_offset;
    int action_is_void;
    int action_is_disabled;
    dnx_field_action_t dnx_action;
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    int is_action_user_defined;
    int payload_result_buffer_shift;
    dbal_fields_e dnx_action_dbal_field;
    uint32 encoded_dbal_dnx_action_val[BITS2WORDS((DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FES_ACTION + 1))] =
        { 0 };

    SHR_FUNC_INIT_VARS(unit);

    field_stage = fg_info_p->group_basic_info.field_stage;
    fg_type = fg_info_p->group_basic_info.fg_type;
    dnx_action = fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action;

    if ((fg_type != DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM) && (attach_full_info_p == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only an external TCAM field group can be called for this "
                     "function without attach_full_info_p. fg_id %d, context_id %d, fg_type %d.\r\n",
                     fg_id, context_id, fg_type);
    }

    action_valid = FALSE;
    ent_action_iter = 0;

    if (is_hit_found)
    {
        /** Iterate over entry actions array and check, which actions are valid. */
        for (ent_action_iter = 0; ent_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
             entry_info_p->payload_info.action_info[ent_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
             ent_action_iter++)
        {
            if (dnx_action == entry_info_p->payload_info.action_info[ent_action_iter].dnx_action)
            {
                action_valid = TRUE;
                break;
            }
        }
    }

    sal_strncpy_s(action_val_exp_string, "", sizeof(action_val_exp_string));

    /** Get action size, base action and BCM equivalent for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, dnx_action, &action_size));
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, field_stage, dnx_action, &bcm_action));
    /**
     * Check if the action is user defined, then we should get its base action.
     */
    is_action_user_defined = (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER);
    if (is_action_user_defined)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action(unit, field_stage, dnx_action, &base_dnx_action));
    }
    else
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }

    /** Check if the action is void.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, field_stage, dnx_action, &action_is_void));

    /** Check if the action is disabled. */
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM)
    {
        if (attach_full_info_p->attach_basic_info.action_info[action_iter].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            if (action_is_void)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Action number %d (%s, %d) in fg_id %d, context_id %d has "
                             "invalidating priority but is a void action.\r\n",
                             action_iter, dnx_field_bcm_action_text(unit, bcm_action), bcm_action, fg_id, context_id);
            }
            action_is_disabled = TRUE;
        }
        else
        {
            action_is_disabled = FALSE;
        }
    }
    else
    {
        if (action_is_void == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action number %d (%s, %d) in fg_id %d, context_id %d isn't void action, "
                         "even though field group is of type external TCAM.\r\n",
                         action_iter, dnx_field_bcm_action_text(unit, bcm_action), bcm_action, fg_id, context_id);
        }
        action_is_disabled = FALSE;
    }

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        /** Set 'FES/FEM Id' cell. */
        if (action_is_void || action_is_disabled)
        {
            PRT_CELL_SET("%s", "None");
            if (action_is_void == FALSE)
            {
                (*action_non_void_iter_p)++;
            }
        }
        else
        {
            PRT_CELL_SET("FES %d",
                         attach_full_info_p->actions_fes_info.initial_fes_quartets[*action_non_void_iter_p].fes_id);
            (*action_non_void_iter_p)++;
        }
    }

    /** Set 'Action Type' cell. */
    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, dnx_action));
    /** If the action is user defined, then we print the base action type. */
    /** Set 'Base Action (ID)' cell. */
    if (is_action_user_defined)
    {
        PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                     (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id, dnx_action, &action_offset));
    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &payload_result_buffer_shift));

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        /** Set 'Valid/Cndition bit' cell. */
        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit)
        {
            if (action_is_void)
            {
                PRT_CELL_SET("%s", "N/A");
            }
            else if (action_is_disabled)
            {
                PRT_CELL_SET("%s", "Disabled");
            }
            else
            {
                PRT_CELL_SET("%s", "Always Valid");
            }
        }
        else
        {
            if (action_is_void)
            {
                PRT_CELL_SET("%s", "N/A");
            }
            else if (action_is_disabled)
            {
                PRT_CELL_SET("%s", "Disabled");
            }
            else
            {
                uint32 action_valid_bit_val;
                /** Extract only the valid bit value from the result_values_p for current action. */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                (result_values_p, (action_offset - 1) + payload_result_buffer_shift, 1,
                                 &action_valid_bit_val));

                PRT_CELL_SET("%d (valid if %d)", action_valid_bit_val,
                             attach_full_info_p->attach_basic_info.action_info[action_iter].valid_bit_polarity);
            }
        }
    }

    if (action_valid)
    {
        uint8 cur_action_result_buff_size = 0;
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "0x%X",
                                              entry_info_p->payload_info.action_info[ent_action_iter].action_value[0]);
        sal_snprintf(action_val_exp_string, sizeof(action_val_exp_string), "0x%X",
                     entry_info_p->payload_info.action_info[ent_action_iter].action_value[0]);

        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit)
        {
            cur_action_result_buff_size = (*start_bit_p + action_size);
        }
        else
        {
            cur_action_result_buff_size = (*start_bit_p + action_size + 1);
        }

        if (cur_action_result_buff_size > dnx_data_field.group.payload_max_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Potential buffer overflow was found, when trying to set 'action_result_buff_p' for action %s! "
                         "action_result_buff_p current size %d, action_size %d, action_result_buff_p max size %d, use_valid_bit %d.\r\n",
                         dnx_field_dnx_action_text(unit, dnx_action), *start_bit_p, action_size,
                         dnx_data_field.group.payload_max_size_get(unit),
                         !(fg_info_p->group_full_info.actions_payload_info.
                           actions_on_payload_info[action_iter].dont_use_valid_bit));
        }

        if (is_action_user_defined)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                            (unit, field_stage, base_dnx_action, &dnx_action_dbal_field));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, field_stage, dnx_action, &dnx_action_dbal_field));
        }

        if (dbal_fields_is_field_encoded(unit, dnx_action_dbal_field))
        {
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, dnx_action_dbal_field,
                                                          entry_info_p->payload_info.
                                                          action_info[ent_action_iter].action_value[0],
                                                          encoded_dbal_dnx_action_val));
        }
        else
        {
            encoded_dbal_dnx_action_val[0] = entry_info_p->payload_info.action_info[ent_action_iter].action_value[0];
        }

        /** In case we use the valid bit, we should add 1 to the action start_bit_p. */
        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit)
        {
            /** Store the action result values in the action_result_buff_p. */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (encoded_dbal_dnx_action_val, *start_bit_p, action_size, action_result_buff_p));
            *start_bit_p += action_size;
        }
        else
        {
            /** Add valid bit value to the action value. */
            *encoded_dbal_dnx_action_val =
                ((*encoded_dbal_dnx_action_val << 1) +
                 attach_full_info_p->attach_basic_info.action_info[action_iter].valid_bit_polarity);
            /** Store the action result values in the action_result_buff_p. */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (encoded_dbal_dnx_action_val, *start_bit_p, (action_size + 1), action_result_buff_p));
            *start_bit_p += action_size + 1;
        }

        /** Set 'Entry Payload Value' cell. */
        if (fg_type == DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM)
        {
            if (is_hit_found)
            {
                PRT_CELL_SET("%s", action_val_exp_string);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }
        else
        {
            if (action_is_void || action_is_disabled)
            {
                if (is_hit_found)
                {
                    PRT_CELL_SET("%s", action_val_exp_string);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
            }
            else
            {
                uint32 entry_payload_action_value = 0;

                /** Get the current entry action value from the action_result_buff_p and result_values_p. */
                if (fg_info_p->group_full_info.actions_payload_info.
                    actions_on_payload_info[action_iter].dont_use_valid_bit)
                {
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                    (result_values_p, action_offset + payload_result_buffer_shift, action_size,
                                     &action_tcam_result_val));
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                    (action_result_buff_p, (*start_bit_p - action_size), action_size,
                                     &entry_payload_action_value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                    (result_values_p, action_offset + payload_result_buffer_shift - 1, action_size,
                                     &action_tcam_result_val));
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                    (action_result_buff_p, (*start_bit_p - action_size - 1), action_size,
                                     &entry_payload_action_value));
                }
                if (is_hit_found)
                {
                    if (DIAG_DNX_FIELD_LAST_IS_SIM)
                    {
                        PRT_CELL_SET("%s", action_val_exp_string);
                    }
                    else
                    {
                        if (entry_payload_action_value == action_tcam_result_val)
                        {
                            PRT_CELL_SET("%s", action_val_exp_string);

                            if (bcm_action == bcmFieldActionAceEntryId)
                            {
                                *action_ace_entry_id_value_p =
                                    entry_info_p->payload_info.action_info[ent_action_iter].action_value[0];
                            }
                        }
                        else
                        {
                            *mismatch_act_error_print_p = TRUE;
                            LOG_CLI_EX
                                ("Mismatch between %s action values, result 0x%X, entry payload 0x%X\r\n %s",
                                 dnx_field_dnx_action_text(unit, dnx_action), action_tcam_result_val,
                                 entry_payload_action_value, EMPTY);
                        }
                    }
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
            }
        }

    }
    else
    {
        if (is_hit_found)
        {
            /** Set 'Entry Payload Value' cell. */
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            /** Set 'Entry Payload Value' cell. */
            PRT_CELL_SET("%s", "N/A");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FES actions of a DE FGs.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - DE Field group ID.
 * \param [in] action_iter - Number of the current action in field group.
 * \param [in] action_size - Id of the current action.
 * \param [in] action_lsb - Lsb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different.
 * \param [in] action_msb - Msb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [in,out] action_non_void_iter_p - Number of current action amongst non void actions.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_action_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    int action_iter,
    unsigned int action_size,
    unsigned int action_lsb,
    unsigned int action_msb,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr,
    int *action_non_void_iter_p)
{
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int action_offset;
    uint32 action_pkt_val = 0;
    uint32 action_key_val = 0;
    uint32 action_valid_bit_val;
    bcm_field_action_info_t action_info;
    char action_type_column_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int action_prefix_value[1];
    int action_is_void;
    int action_is_disabled;
    dnx_field_action_t dnx_action;
    int is_action_user_defined;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy_s(action_val_string, "", sizeof(action_val_string));
    action_offset = 0;

    dnx_action = fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action;

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id, dnx_action, &action_offset));

    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action,
                     &bcm_action));

    /**
     * Check if the action is user defined, then we should get its base action and prefix value.
     */
    is_action_user_defined = (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER);
    if (is_action_user_defined)
    {
        SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, bcm_action, bcmFieldStageCount, &action_info));

        action_prefix_value[0] = action_info.prefix_value;

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                        (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &base_dnx_action));
    }
    else
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }

    /** Check if the action is void.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                    (unit, fg_info_p->group_basic_info.field_stage, dnx_action, &action_is_void));

    /** Check if the action is disabled.*/
    if (attach_full_info_p->attach_basic_info.action_info[action_iter].priority == BCM_FIELD_ACTION_INVALIDATE)
    {
        if (action_is_void)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action number %d (%s, %d) in fg_id %d, context_id %d has "
                         "invalidating priority but is a void action.\r\n",
                         action_iter, dnx_field_bcm_action_text(unit, bcm_action), bcm_action, fg_id, context_id);
        }
        action_is_disabled = TRUE;
    }
    else
    {
        action_is_disabled = FALSE;
    }

    /** Set 'FES/FEM Id' cell. */
    if (action_is_void || action_is_disabled)
    {
        PRT_CELL_SET("%s", "None");
        if (action_is_void == FALSE)
        {
            (*action_non_void_iter_p)++;
        }
    }
    else
    {
        PRT_CELL_SET("FES %d",
                     attach_full_info_p->actions_fes_info.initial_fes_quartets[*action_non_void_iter_p].fes_id);
        (*action_non_void_iter_p)++;
    }

    if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dont_use_valid_bit)
    {
        if (action_lsb != 0 || action_msb != 0)
        {
            /** Set current action information. */
            sal_snprintf(action_type_column_string, sizeof(action_type_column_string), "%s [%d:%d]",
                         dnx_field_dnx_action_text(unit, dnx_action), action_lsb, action_msb);
        }
        else
        {
            /** Set current action information. */
            sal_snprintf(action_type_column_string, sizeof(action_type_column_string), "%s",
                         dnx_field_dnx_action_text(unit, dnx_action));
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (key_values_p,
                         (action_offset +
                          fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key), action_size,
                         &action_key_val));

        /** If the action is user defined, then we print the base action type and prefix. */
        if (is_action_user_defined)
        {
            if (action_info.prefix_size > 0)
            {
                sal_strncat_s(action_type_column_string, " +p", sizeof(action_type_column_string));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&action_key_val, 0, action_info.size, &action_pkt_val));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                ((uint32 *) action_prefix_value, action_info.size, action_info.prefix_size,
                                 &action_pkt_val));
            }
            else
            {
                action_pkt_val = action_key_val;
            }

            /** Set 'Action Type c - condition p - prefix' cell. */
            PRT_CELL_SET("%s", action_type_column_string);
            /** Set 'Base Action (UDA ID)' cell. */
            PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                         (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
        }
        else
        {
            /** Set 'Action Type c - condition p - prefix' cell. */
            PRT_CELL_SET("%s", action_type_column_string);
            /** Set 'Base Action (UDA ID)' cell. */
            PRT_CELL_SET("%s", "N/A");

            action_pkt_val = action_key_val;
        }

        /** Set 'Action Value' cell. */
        PRT_CELL_SET("0x%X", action_pkt_val);

        /** Set 'Valid bit / Condition' cell. */
        if (action_is_void)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else if (action_is_disabled)
        {
            PRT_CELL_SET("%s", "Disabled");
        }
        else
        {
            PRT_CELL_SET("%s", "Always Valid");
        }
    }
    else
    {
        if (action_lsb == 0 && action_msb != 0)
        {
            /** Set current action information. */
            sal_snprintf(action_type_column_string, sizeof(action_type_column_string), "%s [%d:%d] +c",
                         dnx_field_dnx_action_text(unit, dnx_action), action_lsb, action_msb);
        }
        else if (action_lsb != 0 && action_msb > action_size)
        {
            /** Set current action information. */
            sal_snprintf(action_type_column_string, sizeof(action_type_column_string), "%s [%d:%d]",
                         dnx_field_dnx_action_text(unit, dnx_action), action_lsb, action_msb);
        }
        else
        {
            /** Set current action information. */
            sal_snprintf(action_type_column_string, sizeof(action_type_column_string), "%s +c",
                         dnx_field_dnx_action_text(unit, dnx_action));
        }

        /** Extract the whole value of the current action from the key, it contains the valid bit. */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (key_values_p,
                         (fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key +
                          action_offset), (action_size - 1), &action_key_val));
        /** Extract only the valid bit value from the key for current action. */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (key_values_p,
                         (fg_info_p->group_full_info.context_info[context_id].key_id.offset_on_first_key +
                          (action_offset - 1)), 1, &action_valid_bit_val));

        /** If the action is user defined, then we print the base action type and prefix. */
        if (is_action_user_defined)
        {
            if (action_info.prefix_size > 0)
            {
                sal_strncat_s(action_type_column_string, " +p", sizeof(action_type_column_string));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&action_key_val, 0, action_info.size, &action_pkt_val));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                ((uint32 *) action_prefix_value, action_info.size, action_info.prefix_size,
                                 &action_pkt_val));
            }
            else
            {
                action_pkt_val = action_key_val;
            }
            /** Set 'Action Type c - condition p - prefix' cell. */
            PRT_CELL_SET("%s", action_type_column_string);
            /** Set 'Base Action (UDA ID)' cell. */
            PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                         (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
        }
        else
        {
            /** Set 'Action Type c - condition p - prefix' cell. */
            PRT_CELL_SET("%s", action_type_column_string);
            /** Set 'Base Action (UDA ID)' cell. */
            PRT_CELL_SET("%s", "N/A");

            action_pkt_val = action_key_val;
        }

        /** Set 'Action Value' cell. */
        PRT_CELL_SET("0x%X", action_pkt_val);

        /** Set 'Valid bit / Condition' cell. */
        if (action_is_void)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else if (action_is_disabled)
        {
            PRT_CELL_SET("%s", "Disabled");
        }
        else
        {
            PRT_CELL_SET("%d (valid if %d)", action_valid_bit_val,
                         attach_full_info_p->attach_basic_info.action_info[action_iter].valid_bit_polarity);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FES actions,
 *   of a specific CONST FGs.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id - Field group ID.
 * \param [in] context_id - Context ID
 * \param [in] action_iter - Id of the current action.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_const_fg_action_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int action_iter,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int action_is_void;
    int action_is_disabled;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy_s(action_val_string, "", sizeof(action_val_string));

    /** Get base action and BCM equivalent for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action,
                     &bcm_action));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action,
                     &base_dnx_action));

    /** Check if the action is void.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, fg_info_p->group_basic_info.field_stage,
                                                     fg_info_p->group_full_info.
                                                     actions_payload_info.actions_on_payload_info[action_iter].
                                                     dnx_action, &action_is_void));

    /** Check if the action is disabled.*/
    if (attach_full_info_p->attach_basic_info.action_info[action_iter].priority == BCM_FIELD_ACTION_INVALIDATE)
    {
        if (action_is_void)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action number %d (%s, %d) in fg_id %d, context_id %d has "
                         "invalidating priority but is a void action.\r\n",
                         action_iter, dnx_field_bcm_action_text(unit, bcm_action), bcm_action, fg_id, context_id);
        }
        action_is_disabled = TRUE;
    }
    else
    {
        action_is_disabled = FALSE;
    }

    /** Set 'FES Id' cell. */
    if (action_is_void || action_is_disabled)
    {
        PRT_CELL_SET("%s", "None");
    }
    else
    {
        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.initial_fes_quartets[action_iter].fes_id);
    }

    /** Set 'Action Type' cell. */
    PRT_CELL_SET("%s",
                 dnx_field_dnx_action_text(unit,
                                           fg_info_p->group_full_info.actions_payload_info.
                                           actions_on_payload_info[action_iter].dnx_action));

    /** Set 'Base Action (ID)' cell. */
    PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                 (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FEM actions of a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] one_pkt - if only 1 packet was injected before invoking the diag.
 * \param [in] context_id - ID of the selected context. Used for invalidated FEMs.
 * \param [in] fg_id - Field Group Id to retrieve the used FEMs for.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] action_result_buff_p - The action result buffer, which is used in case of
 *  TCAM FG and bcmFieldFemExtractionOutputSourceTypeKey, because we are not taking the values
 *  form the key, they are taken from the result buffer.
 * \param [out] fem_start_index_p - The FEM ID index, which was found as allocated,
 *  and which info was presented. Will be checked in the upper function.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fem_actions_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    uint8 one_pkt,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 *key_values_p,
    uint32 *action_result_buff_p,
    dnx_field_fem_id_t * fem_start_index_p,
    prt_control_t * prt_ctr)
{
    uint32 action_val_bitmap[1] = { 0 };
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char action_val_exp_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    dnx_field_fem_map_index_t fem_map_index;
    bcm_field_fem_action_info_t fem_action_info;
    dnx_field_fem_id_t dnx_fem_id, max_nof_fems;
    unsigned int bit_in_fem_action;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    uint32 fem_condition_bits_max;
    uint32 condition_bits_value;
    bcm_field_action_priority_t encoded_position;
    int nof_fem_id_per_array;
    dnx_field_action_position_t fem_position;
    dnx_field_action_array_t fem_array;
    uint8 do_fem_display;

    dnx_field_group_type_e fg_type;
    dnx_field_stage_e fg_stage;
    dnx_field_action_t dnx_action;
    dnx_field_action_type_t dnx_action_id;
    unsigned int context_using_fem;

    SHR_FUNC_INIT_VARS(unit);

    do_fem_display = FALSE;

    /** Get the Field Group type. */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /**
     * Maximum number of FEM 'map indices' in iPMF1/2 == 4
     * This is the number of actions available per FEM
     */
    max_nof_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    fem_condition_bits_max = dnx_data_field.fem.nof_condition_bits_get(unit);
    nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id_per_array;

    /** Set the starting index for dnx_fem_id to be equal to the last allocated FES + '1'. */
    for (dnx_fem_id = *fem_start_index_p; dnx_fem_id < max_nof_fems; dnx_fem_id++)
    {
        /** Check if the context uses the FEM ID. */
        SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using(unit, dnx_fem_id, context_id, fg_id, &context_using_fem));
        if (context_using_fem == FALSE)
        {
            continue;
        }
        /** If a match is found then display all four 'actions' corresponding to this FEM id. */
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t returned_fg_id;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, dnx_fem_id, fem_program_index, &returned_fg_id));
            if (returned_fg_id == DNX_FIELD_GROUP_INVALID || returned_fg_id != fg_id)
            {
                /** This 'fem_program' is not occupied for specified 'fem_id'. Go to the next. */
                continue;
            }

            /** Calculate the fem_array and fem_position from the current dnx_fem_id. */
            fem_array = (((dnx_fem_id / nof_fem_id_per_array) * 2) + 1);
            fem_position = (dnx_fem_id % nof_fem_id_per_array);
            encoded_position = BCM_FIELD_ACTION_POSITION(fem_array, fem_position);
            SHR_IF_ERR_EXIT(bcm_field_fem_action_info_get
                            (unit, (bcm_field_group_t) fg_id, encoded_position, &fem_action_info));

            /**
             * In case of TCAM FG we should extract the 4 condition bits from the action_result_buff_p,
             * otherwise we extract them from the key_values.
             */
            if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
            {
                /** Extract the condition bits value from the key_values. */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                               ((fem_action_info.fem_input.input_offset +
                                                                 (fem_action_info.condition_msb + 1)) -
                                                                fem_condition_bits_max), fem_condition_bits_max,
                                                               &condition_bits_value));
            }
            else
            {
                /** Extract the condition bits value from the action_res_buff. */
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(action_result_buff_p,
                                                               ((fem_action_info.fem_input.input_offset +
                                                                 (fem_action_info.condition_msb + 1)) -
                                                                fem_condition_bits_max), fem_condition_bits_max,
                                                               &condition_bits_value));
            }

            fem_map_index = fem_action_info.fem_condition[condition_bits_value].extraction_id;

            /** Set 'FES/FEM Id' cell. */
            PRT_CELL_SET("FEM %d", dnx_fem_id);
            /** Set 'Action Type' cell. */
            PRT_CELL_SET("%s",
                         dnx_field_bcm_action_text(unit, fem_action_info.fem_extraction[fem_map_index].action_type));
            /** Set 'Base Action (ID)' cell. */
            PRT_CELL_SET("%s", "N/A");

            for (bit_in_fem_action = 0;
                 bit_in_fem_action < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION; bit_in_fem_action++)
            {
                switch (fem_action_info.fem_extraction[fem_map_index].output_bit[bit_in_fem_action].source_type)
                {
                    case bcmFieldFemExtractionOutputSourceTypeKey:
                    {
                        /** Variable to store the value of action bit taken from the key. */
                        uint32 output_key_action_bit_value;

                        if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
                        {
                            /** If the source_type is KEY, we should take the relevant bit from the key. */
                            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                            (key_values_p,
                                             (fem_action_info.fem_input.input_offset +
                                              fem_action_info.
                                              fem_extraction[fem_map_index].output_bit[bit_in_fem_action].offset), 1,
                                             &output_key_action_bit_value));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(action_result_buff_p,
                                                                           (fem_action_info.fem_input.input_offset +
                                                                            fem_action_info.fem_extraction
                                                                            [fem_map_index].output_bit
                                                                            [bit_in_fem_action].offset), 1,
                                                                           &output_key_action_bit_value));
                        }

                        SHR_IF_ERR_EXIT(utilex_bitstream_set
                                        (action_val_bitmap, bit_in_fem_action, output_key_action_bit_value));
                        break;
                    }
                    case bcmFieldFemExtractionOutputSourceTypeForce:
                    {
                        SHR_IF_ERR_EXIT(utilex_bitstream_set(action_val_bitmap, bit_in_fem_action,
                                                             fem_action_info.fem_extraction[fem_map_index].output_bit
                                                             [bit_in_fem_action].forced_value));
                        break;
                    }
                    default:
                    {
                        /** In case no bit is set by the user or taken from the key, set its default value '0'. */
                        SHR_IF_ERR_EXIT(utilex_bitstream_set(action_val_bitmap, bit_in_fem_action, 0));
                        break;
                    }
                }
            }

            action_val_bitmap[0] += fem_action_info.fem_extraction[fem_map_index].increment;

            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "0x%06X", action_val_bitmap[0]);
            sal_snprintf(action_val_exp_string, sizeof(action_val_exp_string), "0x%06X", action_val_bitmap[0]);

            sal_strncpy_s(action_val_string, "", sizeof(action_val_string));

            SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));
            SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, fg_stage,
                                                            fem_action_info.fem_extraction[fem_map_index].action_type,
                                                            &dnx_action));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, fg_stage, dnx_action, &dnx_action_id));

            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
            {
                /** Set '' cell. */
                PRT_CELL_SET("%s", action_val_exp_string);
                /** Set '' cell. */
                PRT_CELL_SET("0x%X", condition_bits_value);

            }
            else
            {
                if (is_hit_found)
                {
                    /** Set 'Valid/Cndition bit' cell. */
                    PRT_CELL_SET("0x%X", condition_bits_value);
                    /** Set 'Entry Payload Value' cell. */
                    PRT_CELL_SET("%s", action_val_exp_string);

                }
                else
                {
                    /** Set 'Valid/Cndition bit' cell. */
                    if (one_pkt)
                    {
                        PRT_CELL_SET("0x%X", condition_bits_value);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "N/A");
                    }
                    /** Set 'Entry Payload Value' cell. */
                    PRT_CELL_SET("%s", "N/A");

                }
            }
            do_fem_display = TRUE;
        }

        /** Return the current FEM ID to be checked in upper function. */
        *fem_start_index_p = dnx_fem_id;

        /**
         * In case we find and set values for the current FEM, we should go to
         * exit and continue presenting information about qualifiers and actions.
         */
        if (do_fem_display)
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about
 *   all Added EFES actions of a specific FG and context.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] one_pkt - if only 1 packet was injected before invoking the diag.
 * \param [in] fg_id - Field group ID.
 * \param [in] context_id - Context ID
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] field_stage - Stage of the FG to which the EFES was added.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] result_values_p - Values of the results returned by signals.
 * \param [out] added_fes_start_index - The added EFES ID index, which was found as allocated,
 *  and which info was presented. Will be checked in the upper function.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_added_efes_action_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    uint8 one_pkt,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_stage_e field_stage,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    uint32 *key_values_p,
    uint32 *result_values_p,
    dnx_field_fes_id_t added_fes_start_index,
    prt_control_t * prt_ctr)
{
    bcm_field_efes_action_info_t efes_action_info;
    uint32 efes_condition_msb_value;
    dnx_field_action_t dnx_action;
    uint32 action_result_val;
    int payload_result_buffer_shift;
    dnx_field_action_position_t fes_position;
    dnx_field_action_array_t fes_array;
    bcm_field_action_priority_t encoded_position;
    dnx_field_group_type_e fg_type;
    int de_group_lsb;
    uint32 action_valid_bit_val;

    SHR_FUNC_INIT_VARS(unit);

    action_valid_bit_val = 0;

    /** Get the Field Group type. */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (added_fes_start_index >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
        || attach_full_info_p->actions_fes_info.added_fes_quartets[added_fes_start_index].fes_id ==
        DNX_FIELD_EFES_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Diag failed on Added EFESs printing! A valid added EFES (%d) has to be presented, but it is marked as INVALID"
                     "in added_fes_quartets[] or exceeded the max number of added EFESs (%d).\r\n",
                     added_fes_start_index, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

    DNX_FIELD_ACTION_FES_ID_ARRAY_POSITION_GET(fes_array, fes_position, unit, field_stage,
                                               attach_full_info_p->
                                               actions_fes_info.added_fes_quartets[added_fes_start_index].fes_id);
    encoded_position = BCM_FIELD_ACTION_POSITION(fes_array, fes_position);
    bcm_field_efes_action_info_t_init(&efes_action_info);
    SHR_IF_ERR_EXIT(bcm_field_efes_action_info_get(unit, fg_id, context_id, encoded_position, &efes_action_info));

    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        /**
         * In case of DE FG we should extract the 2 condition bits from the key_values,
         * otherwise we extract them from the result_values_p.
         */
        SHR_IF_ERR_EXIT(dnx_field_group_context_de_position_get(unit, fg_id, context_id, &de_group_lsb));
        /** Extract the condition bits value from the key_values. */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (key_values_p, ((de_group_lsb + (efes_action_info.condition_msb)) - 1), 2,
                         &efes_condition_msb_value));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &payload_result_buffer_shift));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (result_values_p, ((efes_action_info.condition_msb + payload_result_buffer_shift) - 1), 2,
                         &efes_condition_msb_value));
    }

    if (efes_action_info.is_condition_valid == FALSE)
    {
        /** If is_condition_valid is false, only the first element of 2msb feature is relevant. */
        efes_condition_msb_value = 0;
    }

    /** Set 'FES/FEM Id' cell. */
    PRT_CELL_SET("FES %d (added)",
                 attach_full_info_p->actions_fes_info.added_fes_quartets[added_fes_start_index].fes_id);
    /** Set 'Action Type' cell. */
    if (efes_action_info.efes_condition_conf[efes_condition_msb_value].is_action_valid == FALSE)
    {
        PRT_CELL_SET("%s", "Invalid");
    }
    else
    {
        PRT_CELL_SET("%s",
                     dnx_field_bcm_action_text(unit,
                                               efes_action_info.
                                               efes_condition_conf[efes_condition_msb_value].action_type));
    }
    /** Set 'Base Action (ID)' cell. */
    PRT_CELL_SET("%s", "N/A");

    if (efes_action_info.efes_condition_conf[efes_condition_msb_value].is_action_valid)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, field_stage,
                                                        efes_action_info.efes_condition_conf
                                                        [efes_condition_msb_value].action_type, &dnx_action));

        /** Retrieve the action value. */
        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (key_values_p,
                             efes_action_info.efes_condition_conf[efes_condition_msb_value].action_lsb +
                             de_group_lsb,
                             efes_action_info.efes_condition_conf[efes_condition_msb_value].action_nof_bits,
                             &action_result_val));

            if (efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == TRUE)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                (key_values_p,
                                 ((efes_action_info.efes_condition_conf[efes_condition_msb_value].action_lsb - 1) +
                                  de_group_lsb), 1, &action_valid_bit_val));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (result_values_p,
                             efes_action_info.efes_condition_conf[efes_condition_msb_value].action_lsb +
                             payload_result_buffer_shift,
                             efes_action_info.efes_condition_conf[efes_condition_msb_value].action_nof_bits,
                             &action_result_val));

            if (efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == TRUE)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                (result_values_p,
                                 ((efes_action_info.efes_condition_conf[efes_condition_msb_value].action_lsb - 1) +
                                  payload_result_buffer_shift), 1, &action_valid_bit_val));
            }
        }
        /** Add the mask to the retrieved value. */
        action_result_val =
            (action_result_val | efes_action_info.efes_condition_conf[efes_condition_msb_value].action_or_mask);
    }

    if (is_hit_found)
    {
        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            if (efes_action_info.efes_condition_conf[efes_condition_msb_value].is_action_valid)
            {
                /** Set '' cell. */
                PRT_CELL_SET("0x%X", action_result_val);

                /** Set '' cell. */
                if (efes_action_info.is_condition_valid == FALSE &&
                    efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == FALSE)
                {
                    PRT_CELL_SET("C bits: %s\nV bit: %s", "Always valid", "Always valid");
                }
                else if (efes_action_info.is_condition_valid == TRUE &&
                         efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == FALSE)
                {
                    PRT_CELL_SET("C bits: 0x%X\nV bit: %s", efes_condition_msb_value, "Always valid");
                }
                else if (efes_action_info.is_condition_valid == FALSE &&
                         efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == TRUE)
                {
                    PRT_CELL_SET("C bits: %s\nV bit: %d (valid if %d)", "Always valid", action_valid_bit_val,
                                 efes_action_info.efes_condition_conf[efes_condition_msb_value].valid_bit_polarity);
                }
                else
                {
                    PRT_CELL_SET("C bits: 0x%X\nV bit: %d (valid if %d)", efes_condition_msb_value,
                                 action_valid_bit_val,
                                 efes_action_info.efes_condition_conf[efes_condition_msb_value].valid_bit_polarity);
                }

            }
            else
            {
                /** Set '' cell. */
                PRT_CELL_SET("%s", "N/A");

                /** Set '' cell. */
                if (efes_action_info.is_condition_valid == FALSE)
                {
                    PRT_CELL_SET("C bits: %s", "Always valid");
                }
                else
                {
                    PRT_CELL_SET("C bits: 0x%X", efes_condition_msb_value);
                }
            }
        }
        else
        {
            if (efes_action_info.efes_condition_conf[efes_condition_msb_value].is_action_valid)
            {
                /** Set 'Valid/Cndition bit' cell. */
                if (efes_action_info.is_condition_valid == FALSE &&
                    efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == FALSE)
                {
                    PRT_CELL_SET("C bits: %s\nV bit: %s", "Always valid", "Always valid");
                }
                else if (efes_action_info.is_condition_valid == TRUE &&
                         efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == FALSE)
                {
                    PRT_CELL_SET("C bits: 0x%X\nV bit: %s", efes_condition_msb_value, "Always valid");
                }
                else if (efes_action_info.is_condition_valid == FALSE &&
                         efes_action_info.efes_condition_conf[efes_condition_msb_value].action_with_valid_bit == TRUE)
                {
                    PRT_CELL_SET("C bits: %s\nV bit: %d (valid if %d)", "Always valid", action_valid_bit_val,
                                 efes_action_info.efes_condition_conf[efes_condition_msb_value].valid_bit_polarity);
                }
                else
                {
                    PRT_CELL_SET("C bits: 0x%X\nV bit: %d (valid if %d)", efes_condition_msb_value,
                                 action_valid_bit_val,
                                 efes_action_info.efes_condition_conf[efes_condition_msb_value].valid_bit_polarity);
                }

                /** Set 'Entry Payload Value' cell. */
                PRT_CELL_SET("0x%X", action_result_val);
            }
            else
            {
                /** Set 'Valid/Cndition bit' cell. */
                if (efes_action_info.is_condition_valid == FALSE)
                {
                    PRT_CELL_SET("C bits: %s", "Always valid");
                }
                else
                {
                    PRT_CELL_SET("C bits: 0x%X", efes_condition_msb_value);
                }

                /** Set 'Entry Payload Value' cell. */
                PRT_CELL_SET("%s", "N/A");
            }
        }
    }
    else
    {
        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            /** Set '' cell. */
            PRT_CELL_SET("C bits: 0x%X", efes_condition_msb_value);
            /** Set '' cell. */
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            /** Set '' cell. */
            PRT_CELL_SET("C bits: 0x%X", efes_condition_msb_value);
            /** Set '' cell. */
            PRT_CELL_SET("%s", "N/A");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_field_last_common_fg_key_payload_print(
    int unit,
    uint8 one_pkt,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    int none_fem_occupied,
    dnx_field_fem_id_t nof_allocated_fems,
    dnx_field_fes_id_t nof_allocated_added_efes,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint8 *do_display_any_fg_info_p,
    uint32 *action_ace_entry_id_value_p,
    sh_sand_control_t * sand_control)
{
    uint32 key_index = 0;
    dnx_field_qual_map_in_key_t *key_qual_map_p = NULL;
    uint32 single_key_size;
    uint32 *key_values_p = NULL;
    uint32 *result_values_p = NULL;
    char qual_val_buff[DSIG_MAX_SIZE_STR] = { '0' };
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    char action_val_buff[DSIG_MAX_SIZE_STR] = { '0' };
    uint32 action_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    uint32 lsb_offset_in_tcam_key = 0;
    dnx_field_action_t base_action = DNX_FIELD_ACTION_INVALID;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_INVALID;
    dnx_field_action_t curr_act = DNX_FIELD_ACTION_INVALID;
    dnx_field_qual_t curr_qual = DNX_FIELD_QUAL_ID_INVALID;
    dnx_field_action_in_group_info_t *action_info_p = NULL;
    unsigned int action_size = 0;
    uint32 action_valid_bit_val;
    int action_is_void = FALSE;
    dnx_field_fem_id_t max_num_fems;
    int max_num_actions;
    int max_num_qual_actions;
    int max_num_quals;
    int max_num_added_efes;
    int qual_left, action_left;
    int qual_action_iter;
    int qual_action_non_void_iter = 0;
    uint8 do_display;
    dnx_field_fes_id_t added_efes_iter;
    dnx_field_fem_id_t fem_id;
    int action_is_disabled;
    int payload_result_buffer_shift;
    int curr_qual_offset;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    max_num_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    max_num_added_efes = dnx_data_field.group.nof_action_per_fg_get(unit);
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);
    qual_left = action_left = TRUE;
    do_display = FALSE;
    fem_id = 0;

    key_qual_map_p = fg_info_p->group_full_info.key_template.key_qual_map;
    result_values_p = last_diag_fg_info_p->result_values;
    field_stage = fg_info_p->group_full_info.field_stage;
    action_info_p = fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info;

    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &payload_result_buffer_shift));

    if (last_diag_fg_info_p->entry_hit)
    {
        PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "HIT");
    }
    else
    {
        PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "NO HIT");
    }

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES/\nFEM");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Value");

    /** Print the FG information first, later the Added FEM's and FES's. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions; qual_action_iter++)
    {
        curr_act = action_info_p[qual_action_iter].dnx_action;
        curr_qual = key_qual_map_p[qual_action_iter].qual_type;
        curr_qual_offset = key_qual_map_p[qual_action_iter].lsb + attach_full_info_p->key_id.offset_on_first_key;

        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < max_num_quals && curr_qual == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < max_num_actions && curr_act == DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }

        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Set 'Qual Type' cell. */
            PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, curr_qual));

            if (curr_qual_offset >= single_key_size)
            {
                key_index = 1;
                lsb_offset_in_tcam_key = curr_qual_offset - single_key_size;
            }
            else
            {
                lsb_offset_in_tcam_key = curr_qual_offset;
                key_index = 0;
            }
            key_values_p = last_diag_fg_info_p->key_values[key_index];

            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (key_values_p, lsb_offset_in_tcam_key, key_qual_map_p[qual_action_iter].size, qual_val));
            sand_signal_value_to_str(unit, SAL_FIELD_TYPE_ARRAY32, qual_val, qual_val_buff,
                                     key_qual_map_p[qual_action_iter].size, 0);
            /** Set 'Key Value' cell. */
            PRT_CELL_SET("0x%s", qual_val_buff);
        }
        else
        {
            /** Skip 'Qual Type', 'Key Value' cells. */
            PRT_CELL_SKIP(2);
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left en print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Check if the action is void.*/
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, field_stage, curr_act, &action_is_void));

            if (attach_full_info_p->attach_basic_info.action_info[qual_action_iter].priority ==
                BCM_FIELD_ACTION_INVALIDATE)
            {
                action_is_disabled = TRUE;
            }
            else
            {
                action_is_disabled = FALSE;
            }

            /** Set 'FES/FEM' cell. */
            if (action_is_void || action_is_disabled)
            {
                PRT_CELL_SET("%s", "None");
                if (action_is_void == FALSE)
                {
                    qual_action_non_void_iter++;
                }
            }
            else
            {
                PRT_CELL_SET("FES %d",
                             attach_full_info_p->actions_fes_info.
                             initial_fes_quartets[qual_action_non_void_iter].fes_id);
                qual_action_non_void_iter++;
            }

            /** Set 'Action Type' cell. */
            PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, curr_act));

            /** Set 'Base Action (ID)' cell. */
            if (DNX_ACTION_CLASS(curr_act) == DNX_FIELD_ACTION_CLASS_USER)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action(unit, field_stage, curr_act, &base_action));
                PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, base_action));
            }
            else
            {
                PRT_CELL_SET("N/A");
            }

            /** Only in case of hit entry information will be print. */
            if (last_diag_fg_info_p->entry_hit)
            {
                bcm_field_action_t bcm_action;
                int action_offset;

                /** Payload value read from the singal returned from DB. */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, curr_act, &action_size));
                SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id, curr_act, &action_offset));

                /** Set 'Valid' cell. */
                if (action_is_void == TRUE)
                {
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    if (fg_info_p->group_full_info.actions_payload_info.
                        actions_on_payload_info[qual_action_iter].dont_use_valid_bit)
                    {
                        PRT_CELL_SET("Valid");
                    }
                    else
                    {
                        /** Extract only the valid bit value from the result_values_p for current action. */
                        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                        (result_values_p, ((action_offset - 1) + payload_result_buffer_shift),
                                         1, &action_valid_bit_val));

                        if (action_valid_bit_val == 1)
                        {
                            PRT_CELL_SET("Valid");
                        }
                        else
                        {
                            PRT_CELL_SET("Invalid");
                        }
                    }
                }

                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                (result_values_p, (action_offset + payload_result_buffer_shift), action_size,
                                 action_val));

                SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, field_stage, curr_act, &bcm_action));
                if (bcm_action == bcmFieldActionAceEntryId)
                {
                    *action_ace_entry_id_value_p = action_val[0];
                }
                sand_signal_value_to_str(unit, SAL_FIELD_TYPE_ARRAY32, action_val, action_val_buff, action_size, 0);

                /** Set 'Payload Value' cell. */
                PRT_CELL_SET("0x%s", action_val_buff);

            }
            else
            {
                /** Set 'Valid' cell. */
                PRT_CELL_SET("N/A");
                /** Set 'Payload Value' cell. */
                PRT_CELL_SET("N/A");
            }
        }
        else
        {
            /** Skip 'FES/FEM', 'Action Type', 'Base Action (ID)', 'Valid', 'Payload Value' cells. */
            PRT_CELL_SKIP(5);
        }

        do_display = TRUE;
    }

    if (nof_allocated_added_efes != 0)
    {
        /** Present all Added EFESs. */
        for (added_efes_iter = 0; added_efes_iter < max_num_added_efes; added_efes_iter++)
        {
            if (nof_allocated_added_efes == 0)
            {
                break;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            /** Skip 'Qual Type', 'Key Value' cells. */
            PRT_CELL_SKIP(2);
            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            SHR_IF_ERR_EXIT(diag_dnx_field_last_added_efes_action_print
                            (unit, last_diag_fg_info_p->entry_hit, core_id, one_pkt, fg_id, context_id, block_p, from_p,
                             to_p, fg_info_p->group_basic_info.field_stage, attach_full_info_p, key_values_p,
                             result_values_p, added_efes_iter, prt_ctr));
            /**
             * Reduce the number of allocated added EFESs, to use it as indication
             * if we still have added EFESs to present.
             */
            nof_allocated_added_efes--;

            do_display = TRUE;
        }
    }

    if (!none_fem_occupied)
    {
        while (fem_id < max_num_fems)
        {
            if (nof_allocated_fems == 0)
            {
                break;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            /** Skip 'Qual Type', 'Key Value' cells. */
            PRT_CELL_SKIP(2);
            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /** Print the FEM actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fem_actions_print
                            (unit, last_diag_fg_info_p->entry_hit, core_id, one_pkt, context_id, fg_id, block_p, from_p,
                             to_p, key_values_p, result_values_p, &fem_id, prt_ctr));
            /**
             * Increase the returned fem_id_p, because it is used as starting index for the loop in
             * diag_dnx_field_last_fem_actions_print().
             */
            fem_id += 1;
            /**
             * Reduce the number of allocated FEMs, to use it as indication
             * if we still have FEMs to present.
             */
            nof_allocated_fems--;

            do_display = TRUE;
        }
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info per field group, which key was used and
 *  its key template. Shows all configured qualifiers and actions
 *  with the expected and actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] last_diag_fg_info_p - pointer to information of this specific FG inforamtion
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] none_fem_occupied - Indicates if there are no occupied FEMs.
 * \param [in] nof_allocated_fems - Number of allocated FEMs for current FG.
 * \param [in] nof_allocated_added_efes - Number of allocated added EFESs for current FG and context.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] entry_info_p - Contains all Entry related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [out] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [out] mismatch_act_error_print_p - Indicates if there is any mismatch found during
 *  the actions printing. In case TRUE, should print error in the end of the calling function.
 * \param [out] action_ace_entry_id_value_p - In case current entry contains bcmFieldActionAceEntryId action
 *  the value of it will be stored in action_ace_entry_id_value_p and used to present ACE Info table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_common_fg_key_payload_print_one_pkt(
    int unit,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 is_hit_found,
    int none_fem_occupied,
    dnx_field_fem_id_t nof_allocated_fems,
    dnx_field_fes_id_t nof_allocated_added_efes,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    dnx_field_entry_t * entry_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint8 *do_display_any_fg_info_p,
    uint8 *mismatch_act_error_print_p,
    uint32 *action_ace_entry_id_value_p,
    sh_sand_control_t * sand_control)
{
    int max_num_quals;
    uint32 action_result_buff[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)] = { 0 };
    int start_bit;
    int max_num_actions;
    int max_num_qual_actions;
    int qual_left, action_left, fem_left, added_efes_left;
    uint32 max_num_qual_actions_fems;
    dnx_field_fem_id_t max_num_fems;
    int qual_action_iter;
    int action_non_void_iter = 0;
    dnx_field_fem_id_t fem_id;
    uint8 key_index;
    uint32 single_key_size;
    uint8 add_new_fg_id;
    uint8 do_display;
    dnx_field_fes_id_t added_efes_id;
    int max_num_added_efes;
    uint32 *key_values_p = NULL;
    uint32 *result_values_p = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    max_num_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    max_num_added_efes = dnx_data_field.group.nof_action_per_fg_get(unit);
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);
    max_num_qual_actions_fems = max_num_qual_actions + max_num_fems + max_num_added_efes;
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    start_bit = 0;
    /** Used as flags to indicate if we have any quals/actions(fes, fem, added efes) to print. */
    qual_left = action_left = fem_left = added_efes_left = TRUE;
    add_new_fg_id = TRUE;
    do_display = FALSE;
    fem_id = 0;
    added_efes_id = 0;

    result_values_p = &(last_diag_fg_info_p->result_values[0]);
    /** Iterate over maximum number of qualifiers per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions_fems; qual_action_iter++)
    {
        /** In case the lsb of the current qualifier is more than 160 or equal to it,
         * we should take the data from the second KEY with index '1', otherwise from
         * first KEY with index '0'.
         */
        if ((fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].lsb +
             attach_full_info_p->key_id.offset_on_first_key) >= single_key_size)
        {
            key_index = 1;
        }
        else
        {
            key_index = 0;
        }
        key_values_p = last_diag_fg_info_p->key_values[key_index];

        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < max_num_quals
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type ==
            DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < max_num_actions
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[qual_action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing FEMs:
         *      - fem_id is equal to maximum number of FEMs
         *      - the nof_allcated_fems was reduced to zero during the printing of FEMs
         *      - if they are any occupied FEMs for current FG (none_fem_occupied)
         */
        if ((fem_id == max_num_fems) || (nof_allocated_fems == 0) || none_fem_occupied)
        {
            fem_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing added EFESs:
         *      - added_efes_id is equal to maximum number of added EFESs
         *      - the nof_allocated_added_efes was reduced to zero during the printing of added EFESs
         */
        if ((added_efes_id == max_num_added_efes) || (nof_allocated_added_efes == 0))
        {
            added_efes_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left && !fem_left && !added_efes_left)
        {
            break;
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            if (last_diag_fg_info_p->entry_hit)
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "HIT");
            }
            else
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "NO HIT");
            }
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Value");
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Mask");
            }
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES/\nFEM\nId");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid/\nCondition\nbit");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Payload Value");

            /** Set the flag to FALSE, if we have more then one key for the current FG, we should skip FG ID cell. */
            add_new_fg_id = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Print the qualifiers info. */
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_EXEM
                || fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_exem_qual_print
                                (unit, context_id, is_hit_found, qual_action_iter, key_values_p, fg_info_p,
                                 entry_info_p, prt_ctr));
            }
            else
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_qual_print
                                (unit, fg_id, context_id, is_hit_found, qual_action_iter, key_values_p, fg_info_p,
                                 entry_info_p, prt_ctr));
            }
        }
        else
        {
            /**
             * Skip 4 cells (Qual, Pak Val, Val, Mask) about qualifier info if no more valid qualifiers were found,
             * but we still have valid actions to be printed.
             */
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
            {
                PRT_CELL_SKIP(4);
            }
            else
            {
                /** For EXEM FGs we don't have Mask column. */
                PRT_CELL_SKIP(3);
            }
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left or fem_left or added_efes_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Print the FES actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_action_print
                            (unit, is_hit_found, core_id, qual_action_iter, fg_id, context_id, block_p, from_p, to_p,
                             result_values_p, fg_info_p, attach_full_info_p, entry_info_p, &action_non_void_iter,
                             &start_bit, action_result_buff, mismatch_act_error_print_p, action_ace_entry_id_value_p,
                             prt_ctr));
        }
        else if (fem_left)
        {
            /** Print the FEM actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fem_actions_print(unit, is_hit_found, core_id, TRUE, context_id, fg_id,
                                                                  block_p, from_p, to_p, key_values_p,
                                                                  action_result_buff, &fem_id, prt_ctr));
            /**
             * Increase the returned fem_id_p, because it is used as starting index for the loop in
             * diag_dnx_field_last_fem_actions_print().
             */
            fem_id += 1;
            /**
             * Reduce the number of allocated FEMs, to use it as indication
             * if we still have FEMs to present.
             */
            nof_allocated_fems--;
        }
        else if (added_efes_left)
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_added_efes_action_print
                            (unit, is_hit_found, core_id, TRUE, fg_id, context_id, block_p, from_p, to_p,
                             fg_info_p->group_basic_info.field_stage, attach_full_info_p, key_values_p, result_values_p,
                             added_efes_id, prt_ctr));
            /**
             * Increase the returned added_fes_id, because it is used as starting index for the loop in
             * diag_dnx_field_last_added_efes_action_print().
             */
            added_efes_id += 1;
            /**
             * Reduce the number of allocated added EFESs, to use it as indication
             * if we still have added EFESs to present.
             */
            nof_allocated_added_efes--;
        }
        else
        {
            /**
             * Skip 6 cells (Fes/Fem, Actions, Base act, Valid/Cnd, Perf Val, Val) about action info if no more valid actions were found,
             * but we still have valid qualifiers to be printed.
             */
            PRT_CELL_SKIP(6);
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info for DE field groups, which key was used and
 *  its key template. Shows all configured qualifiers and actions
 *  with the actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] none_fem_occupied - Indicates if there are no occupied FEMs.
 * \param [in] nof_allocated_fems - Number of allocated FEMs for current FG.
 * \param [in] nof_allocated_added_efes - Number of allocated added EFESs for current FG and context.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values - Values of the used Keys.
 * \param [in] result_values_p - Values of the results returned by signals.
 * \param [out] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_fg_key_payload_print(
    int unit,
    int none_fem_occupied,
    dnx_field_fem_id_t nof_allocated_fems,
    dnx_field_fes_id_t nof_allocated_added_efes,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY],
    uint32 *result_values_p,
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int max_num_quals;
    int max_num_actions;
    int max_num_qual_actions;
    int qual_left, action_left, fem_left, added_efes_left;
    int add_qual, add_action;
    uint32 max_num_qual_actions_fems;
    dnx_field_fem_id_t max_num_fems;
    dnx_field_fem_id_t fem_id;
    uint8 key_index;
    uint32 single_key_size;
    int qual_iter, action_iter;
    int action_non_void_iter = 0;
    unsigned int action_size;
    uint8 qual_size;
    int qual_total_size, action_total_size;
    uint8 do_display;
    dnx_field_fes_id_t added_efes_id;
    int max_num_added_efes;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    max_num_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    max_num_added_efes = dnx_data_field.group.nof_action_per_fg_get(unit);
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);
    max_num_qual_actions_fems = max_num_qual_actions + max_num_fems + max_num_added_efes;
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = fem_left = added_efes_left = TRUE;
    add_qual = add_action = TRUE;
    do_display = FALSE;
    fem_id = 0;
    added_efes_id = 0;

    /** Action and qualifier sizes. */
    action_size = qual_size = 0;
    /** Sum of action and qualifier sizes. */
    action_total_size = qual_total_size = 0;

    PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "DE");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES/\nFEM\nId");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type\nc - condition\np - prefix");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (UDA ID)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid bit/\nCondition");

    /** Iterate over maximum number of qualifiers per FG, to retrieve information about these, which are valid. */
    for (qual_iter = 0, action_iter = 0;
         (qual_iter < max_num_qual_actions_fems) && (action_iter < max_num_qual_actions_fems);)
    {
        /** Reset action and qualifier sizes. */
        action_size = qual_size = 0;

        /** In case the lsb of the current qualifier is more than 160 or equal to it,
         * we should take the data from the second KEY with index '1', otherwise from
         * first KEY with index '0'.
         */
        if ((fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb +
             attach_full_info_p->key_id.offset_on_first_key) >= single_key_size)
        {
            key_index = 1;
        }
        else
        {
            key_index = 0;
        }

        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_iter < max_num_quals
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (action_iter < max_num_actions
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing FEMs:
         *      - fem_id is equal to maximum number of FEMs
         *      - the nof_allcated_fems was reduced to zero during the printing of FEMs
         *      - if they are any occupied FEMs for current FG (none_fem_occupied)
         */
        if ((fem_id == max_num_fems) || (nof_allocated_fems == 0) || none_fem_occupied)
        {
            fem_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing added EFESs:
         *      - added_efes_id is equal to maximum number of added EFESs
         *      - the nof_allocated_added_efes was reduced to zero during the printing of added EFESs
         */
        if ((added_efes_id == max_num_added_efes) || (nof_allocated_added_efes == 0))
        {
            added_efes_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left && !fem_left && !added_efes_left)
        {
            break;
        }

        /**
         * In case there are qualifiers or action, present them.
         */
        if (qual_left || action_left)
        {
            /**
             * Add new row with underscore separator in case we have to print
             * qualifier and action on same row. Otherwise add rows without separators.
             */
            if (add_qual && add_action)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            /** Get the size of the current qualifier. */
            if (qual_left)
            {
                qual_size = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;
            }
            /** Get the size of the current action. */
            if (action_left)
            {
                /** Get action size for the current action. */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                              fg_info_p->group_full_info.
                                                              actions_payload_info.actions_on_payload_info[action_iter].
                                                              dnx_action, &action_size));

                /** In case we have valid bit, we should add 1 bit to the actions size. */
                if (!
                    (fg_info_p->group_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].dont_use_valid_bit))
                {
                    action_size += 1;
                }
            }

            /** In case there is many to many relationship of qualifiers and actions */
            if ((qual_total_size + qual_size > action_total_size) && qual_total_size < action_total_size)
            {
                add_action = TRUE;
            }
            if ((action_total_size + action_size > qual_total_size) && action_total_size < qual_total_size)
            {
                add_qual = TRUE;
            }

            /*
             * In case action or qualifier needs to be printed, add their sizes to total size *
             */
            if (add_action)
            {
                action_total_size += action_size;
            }
            if (add_qual)
            {
                qual_total_size += qual_size;
            }
            /**
             * If total size of the qualifiers becomes equal to total size of actions,
             * print the current qualifier and/or action and increase the iterators.
             */
            if (action_total_size == qual_total_size)
            {
                if (add_qual)
                {
                    /** Print the qualifiers info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                    (unit, context_id, qual_iter, qual_size, 0, 0, key_values[key_index], fg_info_p,
                                     prt_ctr));
                    qual_iter++;
                }
                else
                {
                    /** Skip 'Qual Type', 'Qual LSB', 'Qual Size', 'Packet Value' cells. */
                    PRT_CELL_SKIP(4);
                }

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                if (add_action)
                {
                    /** Print the actions info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                    (unit, core_id, context_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p,
                                     to_p, key_values[key_index], fg_info_p, attach_full_info_p, prt_ctr,
                                     &action_non_void_iter));
                    action_iter++;
                }
                else
                {
                    /**
                     * Skip 'FES/FEM Id', 'Action Type c - condition p - prefix', 'Base Action (UDA ID)',
                     * 'Action Value', 'Valid bit / Condition' cells.
                     */
                    PRT_CELL_SKIP(5);
                }
                /** Clear the total size of the qualifiers and actions. */
                qual_total_size = 0;
                action_total_size = 0;
                /** Continue printing next qualifier and action */
                add_qual = TRUE;
                add_action = TRUE;
            }
            /**
             * In case total size of qualifiers is lower than total size of actions, it means
             * that qualifiers are still related to the current action.
             */
            else if (qual_total_size < action_total_size)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter, qual_size, 0, 0, key_values[key_index], fg_info_p,
                                 prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                if (add_action)
                {
                    /** Print the actions info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                    (unit, core_id, context_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p,
                                     to_p, key_values[key_index], fg_info_p, attach_full_info_p, prt_ctr,
                                     &action_non_void_iter));
                    /** Increase the action iterator, but don't print it */
                    add_action = FALSE;
                    action_iter++;
                }
                else
                {
                    /**
                     * Skip 'FES/FEM Id', 'Action Type c - condition p - prefix', 'Base Action (UDA ID)',
                     * 'Action Value', 'Valid bit / Condition' cells.
                     */
                    PRT_CELL_SKIP(5);
                }

                /** Increase the qualifier iterator. */
                qual_iter++;
            }
            /**
             * In case total size of qualifiers is bigger than total size of actions, it means that we
             * have more then one action, which will take its values from the current qualifier.
             */
            else if (qual_total_size > action_total_size)
            {
                if (add_qual)
                {
                    /** Print the qualifiers info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                    (unit, context_id, qual_iter,
                                     fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size, 0, 0,
                                     key_values[key_index], fg_info_p, prt_ctr));
                    /** Increase the qualifier iterator, but don't print it */
                    add_qual = FALSE;
                    qual_iter++;
                }
                else
                {
                    /** Skip 'Qual Type', 'Qual LSB', 'Qual Size', 'Packet Value' cells. */
                    PRT_CELL_SKIP(4);
                }

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, context_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p,
                                 to_p, key_values[key_index], fg_info_p, attach_full_info_p, prt_ctr,
                                 &action_non_void_iter));
                /** Increase the action iterator. */
                action_iter++;
            }
        }
        /**
         * If there are no qualifiers and FES actions to be presented,
         * we are presenting the FEM actions, if any.
         */
        else if (fem_left)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /**
             * Skip 4 cells (Qualifier, Lsb, Size, Pkt Val) about qualifier info if no more valid qualifiers were found,
             * but we still have valid actions to be printed.
             */
            PRT_CELL_SKIP(4);

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /** Print the FEM actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fem_actions_print
                            (unit, FALSE, core_id, TRUE, context_id, fg_id, block_p, from_p, to_p,
                             key_values[key_index], NULL, &fem_id, prt_ctr));
            /**
             * Increase the returned fem_id_p, because it is used as starting index for the loop in
             * diag_dnx_field_last_fem_actions_print().
             */
            fem_id += 1;
            /**
             * Reduce the number of allocated FEMs, to use it as indication
             * if we still have FEMs to present.
             */
            nof_allocated_fems--;
        }
        /**
         * If there are no qualifiers and FES/FEM actions to be presented,
         * we are presenting the added EFES actions, if any.
         */
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /**
             * Skip 4 cells (Qualifier, Lsb, Size, Pkt Val) about qualifier info if no more valid qualifiers were found,
             * but we still have valid actions to be printed.
             */
            PRT_CELL_SKIP(4);

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            SHR_IF_ERR_EXIT(diag_dnx_field_last_added_efes_action_print(unit, TRUE, core_id, TRUE, fg_id, context_id,
                                                                        block_p, from_p, to_p,
                                                                        fg_info_p->group_basic_info.field_stage,
                                                                        attach_full_info_p, key_values[key_index],
                                                                        result_values_p, added_efes_id, prt_ctr));
            /**
             * Increase the returned added_fes_id, because it is used as starting index for the loop in
             * diag_dnx_field_last_added_efes_action_print().
             */
            added_efes_id += 1;
            /**
             * Reduce the number of allocated added EFESs, to use it as indication
             * if we still have added EFESs to present.
             */
            nof_allocated_added_efes--;
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info for CONST field groups.
 *  Which were the performed actions.
 *  with the actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id - Field group ID.
 * \param [in] context_id - Context ID
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [out] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_const_fg_payload_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int max_num_actions;
    int action_iter;
    uint8 add_new_fg_id;
    uint8 do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);

    add_new_fg_id = TRUE;
    do_display = FALSE;

    /** Iterate over maximum number of actions per FG, to retrieve information about these, which are valid. */
    for (action_iter = 0; action_iter < max_num_actions; action_iter++)
    {
        /**
         * Check if the current action is invalid (DNX_FIELD_ACTION_INVALID), we should break
         * the loop and stop printing actions info.
         */
        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            break;
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            PRT_TITLE_SET("Performed Actions for FG ID %d (%s)", fg_id, "CONST");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES Id");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");

            add_new_fg_id = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /** Print the FES actions info. */
        SHR_IF_ERR_EXIT(diag_dnx_field_last_const_fg_action_print
                        (unit, core_id, fg_id, context_id, action_iter, block_p, from_p, to_p, fg_info_p,
                         attach_full_info_p, prt_ctr));

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *  This function displays info per External field group, which key was used and
 *  its key template. Shows all configured qualifiers and actions
 *  with the expected and actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Contains all Entry related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] kbp_info_p - Contains all collected KBP info, like
 *  used keys and values of all segments.
 * \param [out] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [out] mismatch_act_error_print_p - Indicates if there is any mismatch found durring
 *  the actions printing. In case TRUE, should print error in the end of the calling funtion.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_fg_key_payload_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    diag_dnx_field_last_kbp_fg_info_t * kbp_info_p,
    uint8 *do_display_any_fg_info_p,
    uint8 *mismatch_act_error_print_p,
    sh_sand_control_t * sand_control)
{
    int max_num_quals;
    int max_num_actions;
    int max_num_qual_actions;
    int qual_left, action_left;
    int qual_action_iter;
    int action_non_void_iter = 0;
    uint8 add_new_fg_id;
    uint8 do_display;
    uint32 current_key_offset_seg;
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint32 seg_size;
    uint32 action_ace_entry_id_value;
    uint32 opcode_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    add_new_fg_id = TRUE;
    do_display = FALSE;
    current_key_offset_seg = 0;
    seg_size = 0;

    SHR_IF_ERR_EXIT(dnx_field_kbp_group_segment_indices(unit, fg_id, context_id,
                                                        seg_index_on_master_by_qual, qual_index_on_segment,
                                                        qual_offset_on_segment));
    opcode_id = context_id;
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));

    /** Iterate over maximum number of qualifiers per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions; qual_action_iter++)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < max_num_quals
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type ==
            DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < max_num_actions
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[qual_action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            if (is_hit_found)
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "HIT");
            }
            else
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "NO HIT");
            }
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB on master key");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Mask");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Payload Value");

            /** Set the flag to FALSE, if we have more then one key for the current FG, we should skip FG ID cell. */
            add_new_fg_id = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            int current_key_offset_qual;
            int master_key_seg_index;
            uint32 qual_size;

            if (qual_index_on_segment[qual_action_iter] == 0)
            {
                current_key_offset_seg += BYTES2BITS(BITS2BYTES(seg_size));
            }
            master_key_seg_index = seg_index_on_master_by_qual[qual_action_iter];
            if (master_key_seg_index >= nof_segments)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d, opcode_id %d qualifier %d has segment on master key %d, "
                             "but there are only %d segments on master key.\n",
                             fg_id, context_id, qual_action_iter, master_key_seg_index, nof_segments);
            }
            seg_size = BYTES2BITS(ms_key_segments[master_key_seg_index].nof_bytes);
            /** Get the qual size. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, fg_info_p->group_basic_info.field_stage,
                             fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type,
                             NULL, NULL, &qual_size));

            current_key_offset_qual = current_key_offset_seg + qual_offset_on_segment[qual_action_iter];

            SHR_IF_ERR_EXIT(diag_dnx_field_last_external_qual_print
                            (unit, is_hit_found, kbp_info_p->quals_offset_on_master_key[qual_action_iter],
                             current_key_offset_qual, qual_size, qual_action_iter, kbp_info_p->kbp_acl_key_value,
                             fg_info_p, entry_info_p, prt_ctr));
        }
        else
        {
            /**
             * Skip 5 cells (Qual, Qual lsb on Master Key, Pak Val, Val, Mask) about qualifier
             * info if no more valid qualifiers were found, but we still have valid actions to be printed.
             */
            PRT_CELL_SKIP(5);
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left or fem_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            uint32 single_action_result_buff[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)] = { 0 };
            /** Zeroing the start_bit to not cause buffer overflow issues, because it is not used for External Stage. */
            int start_bit = 0;
            /** Print the FES actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_action_print
                            (unit, is_hit_found, core_id, qual_action_iter, fg_id, context_id, block_p, from_p, to_p,
                             NULL, fg_info_p, NULL, entry_info_p, &action_non_void_iter, &start_bit,
                             single_action_result_buff, mismatch_act_error_print_p, &action_ace_entry_id_value,
                             prt_ctr));
        }
        else
        {
            /**
             * Skip 4 cells (Actions, Base act, Perf Val, Val) about action info if no more valid actions were found,
             * but we still have valid qualifiers to be printed.
             */
            PRT_CELL_SKIP(4);
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *  This function displays master key raw value for External stage.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - The KBP opcode used.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_fg_master_key_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    int key_val_iter;
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint8 payload[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];
    uint32 payload_len;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the master key value. */
    SHR_IF_ERR_EXIT(diag_dnx_field_last_external_fg_master_key_get(unit, core_id, context_id, payload, &payload_len));

    PRT_TITLE_SET("Master Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Key");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    for (key_val_iter = 0; key_val_iter < payload_len; key_val_iter++)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%02X", payload[key_val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%02X", payload[key_val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat_s(qual_val_string, val_buff, sizeof(qual_val_string));
    }

    /** Set 'Key Size' cell. */
    PRT_CELL_SET("%d", BYTES2BITS(payload_len));
    /** Set 'Raw Key' cell. */
    PRT_CELL_SET("0x%s", qual_val_string);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
 * \brief
 *  This function displays info per field group.
 *  Shows payload raw value and its size for External stage.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] block_p - Name of PP block, one of FWD2, IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [in] num_fgs - Number of the field groups, which performed lookups.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_external_fg_payload_print(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_context_t context_id,
    dnx_field_group_t * fg_id_p,
    int num_fgs,
    sh_sand_control_t * sand_control)
{
    int fg_iter;
    int payload_val_iter;
    uint32 payload_sig_value[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS] = { 0 };
    dnx_field_group_full_info_t *fg_info = NULL;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    unsigned int payload_size_in_bits;
    uint32 external_payload_size;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    payload_size_in_bits = 0;
    external_payload_size = 0;

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /** Retrieve the payload value from the relevant signal. */
    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, core_id, block_p, from_p, to_p, "Elk_Lkp_Payload", payload_sig_value,
                     DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS));

    PRT_TITLE_SET("Payload");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Payload");

    /** Iterate over field group array. */
    for (fg_iter = 0; fg_iter < num_fgs; fg_iter++)
    {
        char payload_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        uint32 fg_payload_value[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS] = { 0 };
        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_p[fg_iter], fg_info));

        /** Print payload info only for External field groups. */
        if (fg_info->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id_p[fg_iter], context_id, attach_full_info));

        /** Calculates the "used" payload_size, by the user. */
        SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, fg_info->group_basic_info.field_stage,
                                                              fg_info->group_full_info.
                                                              actions_payload_info.actions_on_payload_info,
                                                              &payload_size_in_bits));
        /** Size of the payload should be byte aligned. */
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, payload_size_in_bits, &external_payload_size));

        /** Extract the payload value, for current FG, from the payload_sig_value. */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (payload_sig_value, attach_full_info->attach_basic_info.payload_offset, external_payload_size,
                         fg_payload_value));

        for (payload_val_iter = (BITS2WORDS(external_payload_size) - 1); payload_val_iter >= 0; payload_val_iter--)
        {
            char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", fg_payload_value[payload_val_iter]);
            sal_snprintf(val_buff, sizeof(val_buff), "%08X", fg_payload_value[payload_val_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(payload_val_string, "%s", val_buff);
            sal_strncat_s(payload_val_string, val_buff, sizeof(payload_val_string));
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /** Set 'FG id' cell. */
        PRT_CELL_SET("%d", fg_id_p[fg_iter]);
        /** Set 'Payload size' cell. */
        PRT_CELL_SET("%d", external_payload_size);
        /** Set 'Raw Payload' cell. */
        PRT_CELL_SET("0x%s", payload_val_string);
    }

    PRT_COMMITX;

exit:
    SHR_FREE(fg_info);
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */

/**
 * \brief
 *  This function displays ACE related info.
 *
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] action_ace_entry_id_value - Value of bcmFieldActionAceEntryId action to
 *  be compared with the CUD_Out_LIF_or_MCDB_Ptr signal value.
 * \param [out] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_ace_info_print(
    int unit,
    bcm_core_t core_id,
    uint32 action_ace_entry_id_value,
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int action_iter, ent_action_iter;
    int do_cell_set;
    unsigned int action_size;
    uint8 do_display;
    dnx_field_ace_id_t ace_format_id;
    dnx_field_ace_format_full_info_t *ace_format_id_info = NULL;
    dnx_field_entry_payload_t entry_payload;
    dnx_field_action_t base_dnx_action;
    dnx_field_action_t dnx_action;
    bcm_field_action_t bcm_action;
    uint32 erpp_cud_out_lif_sig_value = 0;
    uint32 etpp_cud_out_lif_sig_value = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** If the action_ace_entry_id_value wasn't updated then just go to exit. Otherwise get the CUD_Out_LIF signal value. */
    if (action_ace_entry_id_value == -1)
    {
        SHR_EXIT();
    }
    else
    {
        /** Retrieve the CUD_Out_LIF_or_MCDB_Ptr signal value for ERPP block. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "ERPP", "EPMF", "", "CUD_Out_LIF_or_MCDB_Ptr", &erpp_cud_out_lif_sig_value, 1));
        /** Retrieve the CUD_Out_LIF_or_MCDB_Ptr signal value for ETPP block. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "ETPP", "ETParser", "", "CUD_Out_LIF_or_MCDB_Ptr", &etpp_cud_out_lif_sig_value,
                         1));
    }

    if ((dnx_debug_block_is_ready(unit, core_id, DNX_PP_BLOCK_ETPP) != _SHR_E_NONE) ||
        (action_ace_entry_id_value != erpp_cud_out_lif_sig_value) ||
        (erpp_cud_out_lif_sig_value != etpp_cud_out_lif_sig_value))
    {
        LOG_CLI_EX("\r\n" "ACE info can't be presented. !!!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    do_display = FALSE;
    SHR_ALLOC(ace_format_id_info, sizeof(dnx_field_ace_format_full_info_t), "ace_format_id_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &entry_payload));
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_get(unit, action_ace_entry_id_value, &entry_payload, &ace_format_id));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, ace_format_id, ace_format_id_info));

    PRT_TITLE_SET("ACE Format %d  |  ACE Entry 0x%x", ace_format_id, action_ace_entry_id_value);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid bit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Value");

    for (action_iter = 0; action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         ace_format_id_info->format_basic_info.dnx_actions[action_iter] != DNX_FIELD_ACTION_INVALID; action_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        dnx_action = ace_format_id_info->format_basic_info.dnx_actions[action_iter];

        /** Get action size and name for the current action. */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, DNX_FIELD_STAGE_ACE, dnx_action, &action_size));
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_ACE, dnx_action, &bcm_action));

        /** Set 'Action Type' cell. */
        PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, dnx_action));
        /** Set 'Base Action (ID)' cell. */
        if (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                            (unit, DNX_FIELD_STAGE_ACE, dnx_action, &base_dnx_action));
            PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                         (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        /** Set 'Size' cell. */
        PRT_CELL_SET("%d", action_size);
        /** Set 'LSB' cell. */
        PRT_CELL_SET("%d",
                     ace_format_id_info->format_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].lsb);

        do_cell_set = FALSE;

        /** Iterate over entry actions array and check, which actions are valid. */
        for (ent_action_iter = 0; ent_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
             ent_action_iter++)
        {
            /**
             * In case we have no match between current ACE Format action and any of ACE Entry actions,
             * this means that the action is not valid for the entry,
             * we should print '-' (dash) after the for loop.
             */
            if (entry_payload.action_info[ent_action_iter].dnx_action == DNX_FIELD_ACTION_INVALID)
            {
                break;
            }
            else if (ace_format_id_info->format_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].dnx_action ==
                     entry_payload.action_info[ent_action_iter].dnx_action)
            {
                /**
                 * In case we have match between current ACE Format action and any of ACE Entry actions,
                 * this means that the action is valid for the entry. Set the do_cell_set to be TRUE and
                 * prepare the action value for printing after the for loop.
                 */
                do_cell_set = TRUE;
                break;
            }
        }

        if (do_cell_set)
        {
            /** Set 'Valid bit' cell. */
            PRT_CELL_SET("%d",
                         !(ace_format_id_info->format_full_info.actions_payload_info.
                           actions_on_payload_info[action_iter].dont_use_valid_bit));
            /** Set 'Entry Value' cell. */
            PRT_CELL_SET("0x%X", entry_payload.action_info[ent_action_iter].action_value[0]);
        }
        else
        {
            /** Set 'Valid bit' cell. */
            PRT_CELL_SET("%s", "-");
            /** Set 'Entry Value' cell. */
            PRT_CELL_SET("%s", "-");
        }
        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    SHR_FREE(ace_format_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info per field group.
 *  Shows all configured qualifiers and performed actions with the expected
 *  and actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] one_pkt - if only 1 packet was injected before inkoking the diag
 * \param [in] visibility_mode - Visibility mode, will be used for IPMF3 context and DE_INFO presenting.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [in] num_fgs - Number of the field groups, which performed lookups.
 * \param [in] names_p - Names of the blocks for signals
 * \param [in] last_diag_fg_info_p - Contains Key IDs, their values and entries which were hit per FG.
 * \param [out] mismatch_act_error_print_p - Indicates if there is any mismatch found during
 *  the actions printing. In case TRUE, should print error in the end of the calling function.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_key_payload_print(
    int unit,
    bcm_core_t core_id,
    uint8 one_pkt,
    bcm_instru_vis_mode_control_t visibility_mode,
    dnx_field_context_t context_id,
    dnx_field_group_t * fg_id_p,
    int num_fgs,
    diag_dnx_field_utils_sig_block_names_t * names_p,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *mismatch_act_error_print_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_entry_t entry_info;
    int fg_iter;
    dnx_field_group_full_info_t *fg_info = NULL;
    dnx_field_group_type_e fg_type;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    uint8 do_display_any_fg_info;
    int none_fem_occupied;
    uint8 is_hit_found;
    dnx_field_fem_id_t nof_allocated_fems;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_fes_id_t nof_allocated_added_efes;
    shr_error_e rv;
    char *block_p = names_p->block;
    char *from_p = names_p->stage;
    char *to_p = names_p->to;
    uint32 action_ace_entry_id_value = -1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    do_display_any_fg_info = FALSE;

    /** Iterate over field group array. */
    for (fg_iter = 0; fg_iter < num_fgs; fg_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_p[fg_iter], &fg_type));

        /**
         * In case the entry ID of the current allocated FG is invalid, we don't have a hit,
         * otherwise we have a hit and valid entry ID.
         */
        if (last_diag_fg_info_p[fg_iter].entry_id != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            is_hit_found = TRUE;
        }
        else
        {
            is_hit_found = FALSE;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_p[fg_iter], fg_info));
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id_p[fg_iter], context_id, attach_full_info));

        /** Check if any FEM actions are used by the current FG. */
        SHR_IF_ERR_EXIT(dnx_field_fem_is_any_fem_occupied_on_fg
                        (unit, DNX_FIELD_CONTEXT_ID_INVALID, fg_id_p[fg_iter], DNX_FIELD_GROUP_INVALID,
                         DNX_FIELD_IGNORE_ALL_ACTIONS, &none_fem_occupied));
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_nof_alloc_fems_per_fg_get
                        (unit, context_id, fg_id_p[fg_iter], &nof_allocated_fems));
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_nof_alloc_added_efes_per_fg_ctx_get
                        (unit, context_id, fg_id_p[fg_iter], &nof_allocated_added_efes));

        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));

        /** Print information only for supported FG Types. */
        switch (fg_type)
        {
            case DNX_FIELD_GROUP_TYPE_TCAM:
            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            {
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get
                                    (unit, fg_id_p[fg_iter], last_diag_fg_info_p[fg_iter].entry_id, &entry_info));
                }
                if (one_pkt)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print_one_pkt
                                    (unit, &last_diag_fg_info_p[fg_iter], is_hit_found, none_fem_occupied,
                                     nof_allocated_fems, nof_allocated_added_efes, core_id, context_id,
                                     fg_id_p[fg_iter], fg_info, attach_full_info, &entry_info, block_p, from_p, to_p,
                                     &do_display_any_fg_info, mismatch_act_error_print_p, &action_ace_entry_id_value,
                                     sand_control));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print
                                    (unit, one_pkt, &last_diag_fg_info_p[fg_iter], none_fem_occupied,
                                     nof_allocated_fems, nof_allocated_added_efes, core_id, context_id,
                                     fg_id_p[fg_iter], fg_info, attach_full_info, block_p, from_p, to_p,
                                     &do_display_any_fg_info, &action_ace_entry_id_value, sand_control));
                }
                break;
            }
            case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            {
                /** In case of DE in EPMF, just skip the table for J2C and above devices, cause no info is available. */
                if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EPMF
                    && dnx_data_field.features.dir_ext_epmf_get(unit))
                {
                    continue;
                }

                if (one_pkt == FALSE && visibility_mode != bcmInstruVisModeAlways)
                {
                    LOG_CLI_EX("\r\n"
                               "DirectExtraction FG %d info can be presented only if visibility mode is Always!!!%s%s%s\r\n\n",
                               fg_id_p[fg_iter], EMPTY, EMPTY, EMPTY);
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_fg_key_payload_print
                                    (unit, none_fem_occupied, nof_allocated_fems, nof_allocated_added_efes, core_id,
                                     context_id, fg_id_p[fg_iter], fg_info, attach_full_info, block_p, from_p, to_p,
                                     last_diag_fg_info_p[fg_iter].key_values,
                                     last_diag_fg_info_p[fg_iter].result_values, &do_display_any_fg_info,
                                     sand_control));
                }
                break;
            }
            case DNX_FIELD_GROUP_TYPE_CONST:
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_const_fg_payload_print
                                (unit, core_id, fg_id_p[fg_iter], context_id, fg_info, attach_full_info,
                                 block_p, from_p, to_p, &do_display_any_fg_info, sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_EXEM:
            {
                dnx_field_entry_key_t entry_key;
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_exem_entry_key_info_get
                                    (unit, fg_info, last_diag_fg_info_p[fg_iter].key_values[0],
                                     attach_full_info->key_id.offset_on_first_key, &entry_key));
                    rv = dnx_field_entry_exem_get(unit, fg_id_p[fg_iter], &entry_key, &entry_info);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        is_hit_found = FALSE;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print_one_pkt
                                (unit, &last_diag_fg_info_p[fg_iter], is_hit_found, none_fem_occupied,
                                 nof_allocated_fems, nof_allocated_added_efes, core_id, context_id,
                                 fg_id_p[fg_iter], fg_info, attach_full_info, &entry_info, block_p, from_p, to_p,
                                 &do_display_any_fg_info, mismatch_act_error_print_p, &action_ace_entry_id_value,
                                 sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
            {
                dnx_field_entry_key_t entry_key;
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_exem_entry_key_info_get
                                    (unit, fg_info, last_diag_fg_info_p[fg_iter].key_values[0],
                                     attach_full_info->key_id.offset_on_first_key, &entry_key));
                    rv = dnx_field_entry_mdb_dt_get(unit, fg_id_p[fg_iter], &entry_key, &entry_info);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        is_hit_found = FALSE;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print_one_pkt
                                (unit, &last_diag_fg_info_p[fg_iter], is_hit_found, none_fem_occupied,
                                 nof_allocated_fems, nof_allocated_added_efes, core_id, context_id,
                                 fg_id_p[fg_iter], fg_info, attach_full_info, &entry_info, block_p, from_p, to_p,
                                 &do_display_any_fg_info, mismatch_act_error_print_p, &action_ace_entry_id_value,
                                 sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
            {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(dnx_field_entry_external_tcam_get
                                    (unit, fg_id_p[fg_iter], last_diag_fg_info_p[fg_iter].entry_id, NULL, &entry_info));
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_last_external_fg_key_payload_print
                                (unit, is_hit_found, core_id, context_id,
                                 fg_id_p[fg_iter], fg_info, &entry_info, block_p, from_p, to_p,
                                 &last_diag_fg_info_p[fg_iter].kbp_info, &do_display_any_fg_info,
                                 mismatch_act_error_print_p, sand_control));
#else /* defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Group Type External should not exit, "
                             "as KBP is not compiled, and yet it does!!\r\n");
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
                break;
            }
            case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            default:
            {
                /** None of the supported types. */
                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion
                                (unit, fg_info->group_basic_info.fg_type, converted_fg_type));

                LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by FG Info diagnostic!! %s%s%s\r\n\n",
                           converted_fg_type, EMPTY, EMPTY, EMPTY);
                break;
            }
        }

        if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EPMF)
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_ace_info_print
                            (unit, core_id, action_ace_entry_id_value, &do_display_any_fg_info, sand_control));
        }
    }

    if (!do_display_any_fg_info)
    {
        LOG_CLI_EX("\r\n" "No key info and performed actions were found, to be presented!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fg_info);
    SHR_FREE(attach_full_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about field configurations and performed lookups for
 *   a certain stage.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (External/iPMF1/2/3, ePMF.....).
 * \param [in] core_id - Core Id on which the packet was sent.
 * \param [in] one_pkt - if the user commited that only one packet was injected, additional info will be print.
 * \param [in] visibility_mode - Visibility mode, will be used for IPMF3 context and DE_INFO presenting.
 * \param [out] kbp_hitbit_mismatch_p - Compare result between retrieved hit signal value and searched result.
 *  If TRUE then we have mismatch otherwise FALSE.
 * \param [out] mismatch_act_error_print_p - Indicates if there is any mismatch found during
 *  the actions printing. In case TRUE, should print error in the end of the calling function.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_info_print(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_core_t core_id,
    uint8 one_pkt,
    bcm_instru_vis_mode_control_t visibility_mode,
    uint8 *kbp_hitbit_mismatch_p,
    uint8 *mismatch_act_error_print_p,
    sh_sand_control_t * sand_control)
{
    int num_fgs = 0;
    dnx_field_context_t context_id = DNX_FIELD_CONTEXT_ID_INVALID;
    /** Array to store the FGs, which performed lookups. */
    dnx_field_group_t fg_performed_lookup[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    diag_dnx_field_last_fg_info_t *last_diag_fg_info = NULL;
    diag_dnx_field_utils_sig_block_names_t names;

    SHR_FUNC_INIT_VARS(unit);

    /** Allocating last_diag_fg_info because otherwise coverity sends an error. */
    SHR_ALLOC_SET_ZERO(last_diag_fg_info, (sizeof(*last_diag_fg_info) * dnx_data_field.group.nof_fgs_get(unit)),
                       "last_diag_fg_info", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_info_t_init(unit, last_diag_fg_info));
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_init(unit, &names));
    sal_memset(fg_performed_lookup, DNX_FIELD_GROUP_INVALID, sizeof(fg_performed_lookup));

    /** Print information only for supported Field stages. */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_set
                            (unit, "IRPP", "IPMF1", "FER", "IPMF1", "TCAM", &names));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_set
                            (unit, "IRPP", "IPMF1", "FER", "IPMF2", "TCAM", &names));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_set
                            (unit, "IRPP", "IPMF3", "LBP", "IPMF3", "TCAM", &names));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_set
                            (unit, "ERPP", "EPMF", "ETMR", "EPMF", "TCAM", &names));
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_sig_block_names_t_set
                            (unit, "IRPP", "FWD2", "IPMF1", "FWD2", "IPMF1", &names));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                            (unit, core_id, field_stage, &names, one_pkt, visibility_mode, &context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                            (unit, core_id, field_stage, one_pkt, visibility_mode, context_id, &names,
                             fg_performed_lookup, &num_fgs, last_diag_fg_info, kbp_hitbit_mismatch_p, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                            (unit, core_id, one_pkt, visibility_mode, context_id, fg_performed_lookup, num_fgs, &names,
                             last_diag_fg_info, mismatch_act_error_print_p, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_external_fg_master_key_print(unit, core_id, context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_external_fg_payload_print
                            (unit, core_id, "IRPP", "FWD2", "IPMF1", context_id, fg_performed_lookup, num_fgs,
                             sand_control));
#else
            LOG_CLI_EX("\r\n" "KBP lib is not compiled!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
            SHR_EXIT();
            break;
        }
        default:
        {
            bcm_field_stage_t bcm_stage;
            /** Convert DNX to BCM Field Stage */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Field Stage '%s' is not supported by Last Packet diagnostic!! %s%s%s\r\n\n",
                       dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
            break;
        }
    }

    SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                    (unit, core_id, field_stage, &names, one_pkt, visibility_mode, &context_id, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                    (unit, core_id, field_stage, one_pkt, visibility_mode, context_id, &names, fg_performed_lookup,
                     &num_fgs, last_diag_fg_info, kbp_hitbit_mismatch_p, sand_control));
    /** No support for FG info print on adapter for iPMF3 stage. */
    if (DIAG_DNX_FIELD_LAST_IS_SIM != TRUE || field_stage != DNX_FIELD_STAGE_IPMF3)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                        (unit, core_id, one_pkt, visibility_mode, context_id, fg_performed_lookup, num_fgs, &names,
                         last_diag_fg_info, mismatch_act_error_print_p, sand_control));
    }

exit:
    SHR_FREE(last_diag_fg_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about field configurations and performed lookups for
 *   a certain stage or range of stages, specified by the caller.
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
diag_dnx_field_last_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;

    bcm_core_t core_id, core_index;
    uint8 num_of_traffic_cores = dnx_data_device.general.nof_cores_for_traffic_get(unit);
    uint8 is_traffic_found;
    uint8 one_pkt;
    bcm_instru_vis_mode_control_t visibility_mode;
    uint8 kbp_hitbit_mismatch;
    uint8 mismatch_act_error_print;
    int external_tcam_exists;

    SHR_FUNC_INIT_VARS(unit);

    kbp_hitbit_mismatch = mismatch_act_error_print = FALSE;

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_INT32(DIAG_DNX_FIELD_OPTION_CORE, core_id);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_ONE_PKT, one_pkt);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    if (DIAG_DNX_FIELD_LAST_IS_SIM)
    {
        bcm_core_t adapter_core = 0;
        adapter_core = dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get(unit);

        if ((core_id != adapter_core) && (core_id != _SHR_CORE_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid CORE was given. CORE values for adapter, can be '0' or 'all'\r\n\n");
        }

        if (one_pkt == FALSE)
        {
            LOG_CLI_EX("\r\n"
                       "adapter last info only supported when only one packet injected i.e. one_pkt=True !!!%s%s%s%s\r\n\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }
    }

    is_traffic_found = FALSE;

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &visibility_mode, FALSE));

    SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &external_tcam_exists));

    /**
     * Iterate over max number of cores and check memory status of entire direction.
     * Used the core ID later to check the signals on it.
     */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        uint8 core_not_presented = TRUE;

        if (core_index >= num_of_traffic_cores)
        {
            continue;
        }

        /** Iterate over all stages. */
        for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
        {
            dnx_field_stage_e field_stage;
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                            (unit, Field_last_diag_stage_enum_table[stage_index].string, &field_stage));

            /**
             * In case of stage External and the KBP lib is not presented
             * we are skipping the External stage.
             */
            if (stage_lower != stage_upper)
            {
                if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
                {
                    if (!external_tcam_exists)
                    {
                        continue;
                    }
                }
            }

            if (field_stage != DNX_FIELD_STAGE_EPMF)
            {
                if (dnx_debug_block_is_ready(unit, core_index, DNX_PP_BLOCK_IRPP) != _SHR_E_NONE)
                {
                    continue;
                }
            }
            else
            {
                if (dnx_debug_block_is_ready(unit, core_index, DNX_PP_BLOCK_ERPP) != _SHR_E_NONE)
                {
                    continue;
                }
            }

            if (core_not_presented)
            {
                bsl_printf("\r\n******************************\r\n");
                bsl_printf("*      CORE ID %d            *\r\n", core_index);
                bsl_printf("******************************\r\n\n");
                core_not_presented = FALSE;
            }

            bsl_printf("\r\n******************************\r\n");
            bsl_printf("*           %s            *\r\n", dnx_field_stage_text(unit, field_stage));
            bsl_printf("******************************\r\n");

            if (DIAG_DNX_FIELD_LAST_IS_SIM == TRUE)
            {
                if ((field_stage == DNX_FIELD_STAGE_EPMF || field_stage == DNX_FIELD_STAGE_EXTERNAL))
                {
                    LOG_CLI_EX("\r\n" "Last Info diag on adapter is not supported for %s stage!!!%s%s%s\r\n\n",
                               dnx_field_stage_text(unit, field_stage), EMPTY, EMPTY, EMPTY);
                    continue;
                }
            }

            SHR_IF_ERR_EXIT(diag_dnx_field_last_info_print
                            (unit, field_stage, core_index, one_pkt, visibility_mode, &kbp_hitbit_mismatch,
                             &mismatch_act_error_print, sand_control));

            is_traffic_found = TRUE;
        }
    }

    if (kbp_hitbit_mismatch || mismatch_act_error_print)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Last Info diag failed! Look above for 'Mismatch' prints.\r\n");
    }

    if (!is_traffic_found)
    {
        LOG_CLI_EX("\r\n" "No Traffic was found !!!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Last (shell commands).
 */
static sh_sand_man_t Field_last_man = {
    .brief = "'Last Info'- displays system view of the entire field lookup process for a certain stage (filter by field stage).",
    .full = "'Last Info' displays system view of the entire field lookup process for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF | EXTERNAL>]"
                "[core=<0 | 1 | all>]"
                "[one_pkt=<Yes | No>]",
    .examples = ""
                "\n" "stage=IPMF1-EXTERNAL core=all"
                "\n" "stage=iPMF3 one_pkt=yes"
                "\n" "stage=ePMF core=0",
    .init_cb = diag_dnx_field_last_init,
    .deinit_cb = diag_dnx_field_last_deinit,
};
static sh_sand_option_t Field_last_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to show system info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_last_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_ONE_PKT,  SAL_FIELD_TYPE_BOOL, "In case only one packet passed in device more info will be printed",   "No"},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'last' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_last_cmds[] = {
    {"info", diag_dnx_field_last_cb, NULL, Field_last_options, &Field_last_man, NULL,
     NULL,}
    ,
    {NULL}
};

/*
 * }
 */
