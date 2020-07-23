/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_ive_eve.c
 *
 * Ingress VLAN editing information of the last received packet
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <appl/diag/system.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/*************
 *  MACROS  *
 */

#define EGRESS_VLAN_EDIT_PROFILE_INVALID (0xFFFFFFFF)

/** prints in table a name of signal and value where parsing is string */
#define __PRT_TABLE_ENTRY_PARSE_STR(__shifts, __entry_name, __entry_value, __parsing, __line_separation)                                   \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("%s (%d)", __parsing, __entry_value);                                                                            \

/** prints in table a name of signal and value where parsing is hex number */
#define __PRT_TABLE_ENTRY_PARSE_INT(__shifts, __entry_name, __entry_value, __parsing, __line_separation)                                   \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("0x%x (%d)", __parsing, __entry_value);                                                                          \

/** prints in table a name of signal and value */
#define __PRT_TABLE_ENTRY(__shifts, __entry_name, __entry_value, __line_separation)                                                        \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                                  \
    PRT_CELL_SET("%d",__entry_value);                                                                                            \

/** prints in table a name of signal and hex value */
#define __PRT_TABLE_ENTRY_HEX(__shifts, __entry_name, __entry_value, __line_separation)                                          \
    PRT_ROW_ADD(__line_separation);                                                                                              \
    PRT_CELL_SET_SHIFT(__shifts, __entry_name);                                                                                  \
    PRT_CELL_SET("0x%x",__entry_value);                                                                                          \


/** returns the tpid_value according to the tpid_idx from the DBAL table GLOBAL_TPID */
#define __GET_TPID_VALUE(__entry_handle_id, __tpid_idx, __tpid_value)                                                            \
if (__tpid_idx < 7)                                                                                                              \
{                                                                                                                                \
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GLOBAL_TPID, __entry_handle_id));                                         \
    dbal_entry_key_field32_set(unit, __entry_handle_id, DBAL_FIELD_TPID_INDEX, __tpid_idx);                                      \
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, __entry_handle_id, DBAL_GET_ALL_FIELDS));                                               \
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get                                                                          \
                    (unit, __entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &__tpid_value));                               \
}                                                                                                                                \

/** check if AC LIF source and print value if yes */
#define __CHECK_IF_SRC_LIF_PRINT(__shifts, _parse_value, _match_string, __print_value)                                                     \
if (sal_strncmp(_parse_value,_match_string, DSIG_MAX_SIZE_STR-1) == 0)                                                           \
{                                                                                                                                \
    __PRT_TABLE_ENTRY_HEX(__shifts, "(LIF_Value)", __print_value, PRT_ROW_SEP_NONE);                                                     \
}                                                                                                                                \

/** check if AC LIF source and print value if yes */
#define __CHECK_IF_SRC_LIF_PRINT_NAME(__shifts, _parse_value, _match_string, __print_value, __print_name)                                  \
if (sal_strncmp(_parse_value,_match_string, DSIG_MAX_SIZE_STR-1) == 0)                                                           \
{                                                                                                                                \
    __PRT_TABLE_ENTRY_HEX(__shifts, __print_name, __print_value, PRT_ROW_SEP_NONE);                                    \
}                                                                                                                                \

/** check signal availability, and skip core_id loop if not available */
#define __CHECK_IF_SIG_NULL_SKIP_CORE_ID(__sig_element, __sig_name, __action_name)                                               \
if (__sig_element == NULL)                                                                                                       \
{                                                                                                                                \
    cli_out("\n%s is not performed for Core_Id=%d\n", __action_name, core_id);                        \
    continue;                                                                                                                    \
}                                                                                                                                \

/*************
 * TYPEDEFS  *
 */

/*************
 * GLOBALS   *
 */
typedef enum
{
    IVE = 0,
    EVE_1 = 1,
    EVE_2 = 2,
    NOF_VLAN_EDITING_STAGES
} vlan_edit_stages_t;
/*************
 * FUNCTIONS *
 */

/**
 * \brief - get the existing number of tags in the last packet in forwarding layer if it's ETHERNET
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] core_id
 *   \param [out] num_of_tags - pointer to the result
 *
 * \par DIRECT OUTPUT:
 *   int
 */
static shr_error_e
dnx_pp_ive_get_num_of_existing_eth_tags(
    int unit,
    int core_id,
    int *num_of_tags)
{
    /** init initial number of tags to 0 */
    int counter = 0;
    uint32 tpid_exist = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * this function is called from IVE and EVE FWD, and if IVE or EVE FWD were performed,
     * the layer of interest would be ETH layer 0, and so the qualifiers would be taken for layer 0
     */
    SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "ETPP", "PRP2", "Term", "epp_layer_qualifiers.0.1st_tpid_exist",
                                      &tpid_exist, 1),
                        "Core_Id %d doesn't contain 'epp_layer_qualifiers.0.1st_tpid_exist'\n", core_id);
    if (tpid_exist != 0)
    {
        counter++;
    }

    SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "ETPP", "PRP2", "Term", "epp_layer_qualifiers.0.2nd_tpid_exist",
                                      &tpid_exist, 1),
                        "Core_Id %d doesn't contain 'epp_layer_qualifiers.0.2nd_tpid_exist'\n", core_id);
    if (tpid_exist != 0)
    {
        counter++;
    }

exit:
    *num_of_tags = counter;

    SHR_FUNC_EXIT;
}

/**
 * \brief - check how many tags are there to add
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] stage
 *   \param [in] key_field_value
 *   \param [out] tpids_to_add
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_pp_ive_get_num_of_tags_to_add(
    int unit,
    vlan_edit_stages_t stage,
    uint32 key_field_value,
    int *tpids_to_add)
{

    dbal_tables_e dbal_table_name;
    dbal_fields_e dbal_key_field_name;

    uint32 entry_handle_id;

    uint32 vid_src_out, vid_src_in;

    int vid_to_add_count = 0;

    /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (stage == IVE)
    {
        dbal_table_name = DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    }
    else if (stage == EVE_1)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    }
    else if (stage == EVE_2)
    {
        dbal_table_name = DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP;
        dbal_key_field_name = DBAL_FIELD_VLAN_EDIT_PROFILE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognizable DBAL VLAN EDIT Table !\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key_field_name, key_field_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (stage == IVE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_OUTER_VID_SRC, vid_src_out, &vid_src_out));
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_INNER_VID_SRC, vid_src_in, &vid_src_in));
    }
    else if ((stage == EVE_1) || (stage == EVE_2))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_VID_SRC, vid_src_out, &vid_src_out));
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_VID_SRC, vid_src_in, &vid_src_in));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognizable DBAL VLAN EDIT Table !\n");
    }

    /** we check that the VID source is not equal to 0 */
    if (vid_src_out)
    {
        vid_to_add_count++;
    }

    if (vid_src_in)
    {
        vid_to_add_count++;
    }

exit:

    *tpids_to_add = vid_to_add_count;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - main function of Ingress VLAN editing information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_ive_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core_in, core_id;

    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    PRT_INIT_VARS;

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        uint32 entry_handle_id;
        int ing_core_id;
        /** check if core_id is valid */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_ETPP, !short_mode);

        /*
         * - Getting the ingress_vlan_edit_command DBAL table info,
         *   which contains the outer/inner tpid,vid,pcp and tags to remove
         * - For each Egress Core_Id we iterate on available Ingress Core IDs,
         *   because only there we can take the VLAN Edit Command, which is key to IVE DBAL,
         *   and we present for each possible Ingress Core_Id the IVE info
         */
        SH_DNX_CORES_ITER(unit, core_in, ing_core_id)
        {
            /** dbal variables */
            uint32 tpid_idx_out, tpid_idx_in;
            uint32 vid_src_out, vid_src_in;
            uint32 pcp_dei_src_out, pcp_dei_src_in;
            uint32 tags_to_remove;
            uint32 vlan_edit_cmd = 0;
            uint32 ac_lif_vid_out = 0;
            uint32 ac_lif_vid_in = 0;
            uint32 ac_lif_dei_out = 0;
            uint32 ac_lif_dei_in = 0;
            uint32 ac_lif_pcp_out = 0;
            uint32 ac_lif_pcp_in = 0;
            uint32 ac_lif_dei_pcp_out = 0;
            uint32 ac_lif_dei_pcp_in = 0;

            uint32 tpid_value_out, tpid_value_in;
            char vid_parse_out[DSIG_MAX_SIZE_STR];
            char vid_parse_in[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_out[DSIG_MAX_SIZE_STR];
            char pcp_dei_parse_in[DSIG_MAX_SIZE_STR];

            /** tpid count variables */
            int tpid_count = 0;
            int ive_num_of_tags_to_add = 0;

            /** check if core_id is valid */
            DNX_PP_BLOCK_IS_READY_CONT(ing_core_id, DNX_PP_BLOCK_IRPP, FALSE);

            /** get the vlan_edit_cmd values - */
            if (dpp_dsig_read(unit, ing_core_id, "IRPP", "IPMF3", "", "VLAN_Edit_Cmd.Cmd", &vlan_edit_cmd, 1)
                != _SHR_E_NONE)
            {
                /** print the warning only if in full mode, otherwise just skip*/
                if (!short_mode)
                {
                    cli_out("\n IVE was not performed for Ingress_Core=%d\n", core_id);
                }
                continue;
            }
            /** If VLAN_Edit_Cmd.Cmd exists - all other must exist as well - so any error is a real one */
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, ing_core_id, "IRPP", "IPMF3", "", "VLAN_Edit_Cmd.VID_1", &ac_lif_vid_out, 1));
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, ing_core_id, "IRPP", "IPMF3", "", "VLAN_Edit_Cmd.VID_2", &ac_lif_vid_in, 1));
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, ing_core_id, "IRPP", "IPMF3", "", "VLAN_Edit_Cmd.DEI_PCP1", &ac_lif_dei_pcp_out, 1));
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, ing_core_id, "IRPP", "IPMF3", "", "VLAN_Edit_Cmd.DEI_PCP2", &ac_lif_dei_pcp_in, 1));

            /** split DEI and PCP fields from the DEI[1]_PCP[3] bitstream */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_out, 0, 1, &ac_lif_dei_out));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_out, 1, 3, &ac_lif_pcp_out));

            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_in, 0, 1, &ac_lif_dei_in));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&ac_lif_dei_pcp_in, 1, 3, &ac_lif_pcp_in));

            /*
             * access the DBAL table and get all IVE information
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, vlan_edit_cmd);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &tpid_idx_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_out));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &tpid_idx_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_in));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE, &tags_to_remove));

            PRT_TITLE_SET("Ingress VLAN Editing Information - Ingress_Core:%d, Egress_Core:%d", ing_core_id, core_id);

            /** Creating columns which support spaces with no shrinking for hierarchy showing */
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");

            /*
             * count the number of tags that are present to check that tags_to_remove doesn't exceed it
             */
            dnx_pp_ive_get_num_of_existing_eth_tags(unit, core_id, &tpid_count);

            if (tags_to_remove > tpid_count)
            {
                PRT_INFO_ADD("**Number of tags to remove is larger than existing tags.");
            }

            /*
             * Parsing the received values for more information to print
             */

            /** TPID index information is present in GLOBAL_TPID table */
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_out, tpid_value_out);
            __GET_TPID_VALUE(entry_handle_id, tpid_idx_in, tpid_value_in);

            /*
             *  get the DBAL text parse of VID, PCP DEI source.
             *  casting 'dbal_field_to_string' because it returns (const char*) and
             *  dnx_dbal_fields_string_form_hw_value_get accepts (char *)
             */

            /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_IVE_OUTER_VID_SRC, vid_src_out, &vid_src_out));
            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_IVE_OUTER_VID_SRC), vid_src_out,
                             vid_parse_out));

            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC, pcp_dei_src_out, &pcp_dei_src_out));
            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC),
                             pcp_dei_src_out, pcp_dei_parse_out));

            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IVE_INNER_VID_SRC, vid_src_in, &vid_src_in));
            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get(unit,
                                                                     (char *) dbal_field_to_string(unit,
                                                                                                   DBAL_FIELD_IVE_INNER_VID_SRC),
                                                                     vid_src_in, vid_parse_in));

            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC, pcp_dei_src_in, &pcp_dei_src_in));
            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC),
                             pcp_dei_src_in, pcp_dei_parse_in));

            SHR_IF_ERR_EXIT(dnx_pp_ive_get_num_of_tags_to_add(unit, IVE, vlan_edit_cmd, &ive_num_of_tags_to_add));

            /** validate that inner tag isn't added before outer */
            if ((vid_src_in) && (!vid_src_out))
            {
                PRT_INFO_ADD("**Inner VID is marked to be added while Outer is not.");
            }

            /*
             * print all the IVE fields in the table
             */
            __PRT_TABLE_ENTRY_HEX(0, "Cmd_Index", vlan_edit_cmd, PRT_ROW_SEP_NONE);
            __PRT_TABLE_ENTRY_PARSE_INT(1, "Outer_TPID_index", tpid_idx_out, tpid_value_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_VID_source", vid_src_out, vid_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_out, "IVE_OUTER_VID_SRC_AC_OUTER_VID", ac_lif_vid_out);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_out, "IVE_OUTER_VID_SRC_AC_INNER_VID", ac_lif_vid_in);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Outer_PCP_DEI_source", pcp_dei_src_out, pcp_dei_parse_out,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_dei_out, "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_pcp_out, "PCP_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_dei_in,
                                          "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_OUTER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_pcp_in,
                                          "PCP_(LIF_Value)");

            __PRT_TABLE_ENTRY_PARSE_INT(1, "Inner_TPID_index", tpid_idx_in, tpid_value_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_VID_source", vid_src_in, vid_parse_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_in, "IVE_INNER_VID_SRC_AC_OUTER_VID", ac_lif_vid_out);
            __CHECK_IF_SRC_LIF_PRINT(2, vid_parse_in, "IVE_INNER_VID_SRC_AC_INNER_VID", ac_lif_vid_in);

            __PRT_TABLE_ENTRY_PARSE_STR(1, "Inner_PCP_DEI_source", pcp_dei_src_in, pcp_dei_parse_in,
                                        PRT_ROW_SEP_UNDERSCORE_BEFORE);
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_dei_out, "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_OUTER_PCP_DEI",
                                          ac_lif_pcp_out, "PCP_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_dei_in,
                                          "DEI_(LIF_Value)");
            __CHECK_IF_SRC_LIF_PRINT_NAME(2, pcp_dei_parse_out, "IVE_INNER_PCP_DEI_SRC_AC_INNER_PCP_DEI", ac_lif_pcp_in,
                                          "PCP_(LIF_Value)");

            __PRT_TABLE_ENTRY(1, "Tags_to_remove", tags_to_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            __PRT_TABLE_ENTRY(0, "Tags_to_add", ive_num_of_tags_to_add, PRT_ROW_SEP_EQUAL_BEFORE);
            __PRT_TABLE_ENTRY(0, "Original_packet_tags_number", tpid_count, PRT_ROW_SEP_UNDERSCORE_BEFORE);

            PRT_COMMITX;

        } /** of ing_core_id structure */

    } /** of for of core_id */

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Collect and print EVE information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] incoming_tag_structure - incoming tage structure
 *   \param [in] nof_tags - ETPS entry ID of the AC entry

 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_eve_nof_tags_from_tag_structure(
    int unit,
    int incoming_tag_structure,
    int *nof_tags)
{
    dbal_enum_value_field_incoming_tag_structure_e field_incoming_tag_structure;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_INCOMING_TAG_STRUCTURE,
                                               incoming_tag_structure, &field_incoming_tag_structure));

    if (field_incoming_tag_structure <= DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_2)
    {
        *nof_tags = 0;
    }
    else if (field_incoming_tag_structure <= DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_2)
    {
        *nof_tags = 2;
    }
    else if (field_incoming_tag_structure <= DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_8)
    {
        *nof_tags = 1;
    }
    else if (field_incoming_tag_structure <= DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_2)
    {
        *nof_tags = 2;
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Can't get nof tags from incoming tag structure (%d, enum %d)\n",
                     incoming_tag_structure, field_incoming_tag_structure);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Collect and print EVE information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] vlan_edit_profile - vlab edit profile
 *   \param [in] ac_etps_id - ETPS entry ID of the AC entry
 *   \param [in] is_native_eve - if the print is for native EVE.
 *   \param [in] sand_control - diagnostic control
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_eve_info_print(
    int unit,
    int core_id,
    uint32 vlan_edit_profile,
    uint32 ac_etps_id,
    uint8 is_native_eve,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint8 is_encap_stage;
    int etps_index;
    int rv;

    /** signal variables */
    uint32 llvp_profile;
    uint32 incoming_tag_structure;
    uint32 lif_outer_vid;
    uint32 lif_inner_vid;
    uint32 forward_domain;
    char sig_name[RHNAME_MAX_SIZE];

    /** dbal variables */
    uint32 eve_vlan_edit_cmd;
    uint32 tpid_idx_out, tpid_idx_in;
    uint32 vid_src_out, vid_src_in;
    uint32 pcp_dei_src_out, pcp_dei_src_in;
    uint32 post_eve_vlan_membership, post_eve_vlan_mirror, post_eve_tx_tag_remove;

    /** parsed variables */
    uint32 tpid_value_out, tpid_value_in;
    char vid_parse_out[DSIG_MAX_SIZE_STR];
    char vid_parse_in[DSIG_MAX_SIZE_STR];
    char pcp_dei_parse_out[DSIG_MAX_SIZE_STR];
    char pcp_dei_parse_in[DSIG_MAX_SIZE_STR];
    char incoming_tag_structure_parse[DSIG_MAX_SIZE_STR];

    /** tags validation */
    uint32 eve_fwd_tags_to_remove;
    int before_eve_num_of_tags = 0;
    int eve_num_of_tags_to_add;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_INIT_VARS;

    if (vlan_edit_profile == EGRESS_VLAN_EDIT_PROFILE_INVALID)
    {
        LOG_CLI(("No Egress%sVLAN Editing on Egress core:%d\n", (is_native_eve ? " Native " : " "), core_id));
        SHR_EXIT();
    }

    /*
     * 1. collect the information related the EVE
     */

    /** Get VID from current ETPS entry*/
    lif_inner_vid = BCM_VLAN_NONE;
    lif_outer_vid = BCM_VLAN_NONE;
    sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "ETPS.%d.VLAN_EDIT_VID_1", ac_etps_id);
    SHR_IF_ERR_EXIT_EXCEPT_IF(dpp_dsig_read(unit, core_id, "ETPP", "Term", "FWD", sig_name, &lif_outer_vid, 2),
                              BCM_E_NOT_FOUND);

    sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "ETPS.%d.VLAN_EDIT_VID_2", ac_etps_id);
    SHR_IF_ERR_EXIT_EXCEPT_IF(dpp_dsig_read(unit, core_id, "ETPP", "Term", "FWD", sig_name, &lif_inner_vid, 2),
                              BCM_E_NOT_FOUND);

    /** Get FODO from previous ETPS entry*/
    forward_domain = BCM_VLAN_NONE;
    for (etps_index = ac_etps_id; etps_index >= 1; etps_index--)
    {
        sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "ETPS.%d.FORWARD_DOMAIN", etps_index);
        rv = dpp_dsig_read(unit, core_id, "ETPP", "Term", "FWD", sig_name, &forward_domain, 1);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        /** The FORWARD_DOMAIN in nearest above ETPS entry is hoped.*/
        break;
    }
    if (forward_domain == BCM_VLAN_NONE)
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "ETPP", "", "", "PPH_Fwd_Domain.value", &forward_domain, 1));
    }

    /** AC is found in ETPS_1, means using in FWD stage, otherwise ENCAP stage*/
    if (ac_etps_id == 1)
    {
        /** If Ac entry is on top of ETPS, it must be used in FWD stage.*/
        is_encap_stage = FALSE;

        /** Get LLVP Profile and Tag Format which are keys to LLVP Table that outputs incoming Tag Structure */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "ETPP", "PRP2", "Term", "per_port_attributes.LLVP_Profile", &llvp_profile, 1));

        /** Get the incoming tag structure which is used to get evec togather with vlan_edit_profile  */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "ETPP", "Term", "FWD", "LLVP_Data.Incoming_Tag_Structure",
                         &incoming_tag_structure, 1));

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get(unit,
                                                                 (char *) dbal_field_to_string(unit,
                                                                                               DBAL_FIELD_INCOMING_TAG_STRUCTURE),
                                                                 incoming_tag_structure, incoming_tag_structure_parse));

        /** convert the in_tag_s HW value (from signal) to ENUM value of this field in DBAL */
        dbal_fields_enum_value_get(unit, DBAL_FIELD_INCOMING_TAG_STRUCTURE, incoming_tag_structure,
                                   &incoming_tag_structure);

        /** Read EVE map table (FWD stage only)*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, vlan_edit_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, incoming_tag_structure);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE, &eve_vlan_edit_cmd));

        /** Read EVE command table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, eve_vlan_edit_cmd);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE, &eve_fwd_tags_to_remove));
    }
    else
    {
        /** If Ac entry is not on top of ETPS, it must be used in ENC stage.*/
        is_encap_stage = TRUE;

        /** Valn Edit Profile is used as EVEC in encap stage*/
        eve_vlan_edit_cmd = vlan_edit_profile;

        /** Read EVE command table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, vlan_edit_profile);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MEMBERSHIP, INST_SINGLE,
                         &post_eve_vlan_membership));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MIRROR, INST_SINGLE, &post_eve_vlan_mirror));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_REMOVE_TX_TAG_ENABLE, INST_SINGLE,
                         &post_eve_tx_tag_remove));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE, &tpid_idx_out));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE, &vid_src_out));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_out));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE, &tpid_idx_in));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE, &vid_src_in));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, INST_SINGLE, &pcp_dei_src_in));

    /*
     * 2. Parsing the received values for more information to print
     */

    /** TPID index information is present in GLOBAL_TPID table */
    __GET_TPID_VALUE(entry_handle_id, tpid_idx_out, tpid_value_out);
    __GET_TPID_VALUE(entry_handle_id, tpid_idx_in, tpid_value_in);

    /*
     *  get the DBAL text parse of VID, PCP DEI source.
     *  casting 'dbal_field_to_string' because it returns (const char*) and
     *  dnx_dbal_fields_string_form_hw_value_get accepts (char *)
     */
    /** since we're dealing with DBAL ENUMs, which return the logical value, we first transfer to hw value */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_OUTER_VID_SRC, vid_src_out, &vid_src_out));
    SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                    (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_OUTER_VID_SRC), vid_src_out,
                     vid_parse_out));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, pcp_dei_src_out, &pcp_dei_src_out));
    SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                    (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC), pcp_dei_src_out,
                     pcp_dei_parse_out));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_EVE_INNER_VID_SRC, vid_src_in, &vid_src_in));
    SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                    (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_INNER_VID_SRC), vid_src_in,
                     vid_parse_in));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, pcp_dei_src_in, &pcp_dei_src_in));
    SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                    (unit, (char *) dbal_field_to_string(unit, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC), pcp_dei_src_in,
                     pcp_dei_parse_in));

    /*
     *  3. Validate the eve command is legal.
     */
    /** Print the table title first.*/
    PRT_TITLE_SET("Egress%sVLAN Editing Information - Egress_Core:%d, Stage:%s", (is_native_eve ? " Native " : " "),
                  core_id, (is_encap_stage ? " ENC " : " FWD "));

    /**  validate that inner tag isn't added before outer */
    if ((vid_src_in) && (!vid_src_out))
    {
        PRT_INFO_ADD("**ERR: inner VID is marked to be added while Outer is not.");
    }

    eve_num_of_tags_to_add = (vid_src_in ? 1 : 0);
    eve_num_of_tags_to_add += (vid_src_out ? 1 : 0);

    if (!is_encap_stage)
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_eve_nof_tags_from_tag_structure
                        (unit, incoming_tag_structure, &before_eve_num_of_tags));

        if (before_eve_num_of_tags < eve_fwd_tags_to_remove)
        {
            PRT_INFO_ADD("**ERR: nof tags to remove is greater than actual nof tags.");
        }
    }
    else
    {
        before_eve_num_of_tags = 0;
    }

    /*
     * 4. print all the EVE FWD fields in the table
     */

    /** Add table column */
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");

    /** Add table content entry by entry */
    __PRT_TABLE_ENTRY_HEX(0, "VLAN_Edit_Profile", vlan_edit_profile, PRT_ROW_SEP_NONE);
    if (!is_encap_stage)
    {
        __PRT_TABLE_ENTRY_PARSE_STR(0, "INCOMING_TAG_STRUCTURE", incoming_tag_structure, incoming_tag_structure_parse,
                                    PRT_ROW_SEP_UNDERSCORE_BEFORE);
    }

    __PRT_TABLE_ENTRY_HEX(1, "Cmd_Index", eve_vlan_edit_cmd, PRT_ROW_SEP_UNDERSCORE_BEFORE);

    /** Outer tag info */
    __PRT_TABLE_ENTRY_PARSE_INT(2, "Outer_TPID_index", tpid_idx_out, tpid_value_out, PRT_ROW_SEP_UNDERSCORE_BEFORE);

    __PRT_TABLE_ENTRY_PARSE_STR(2, "Outer_VID_source", vid_src_out, vid_parse_out, PRT_ROW_SEP_UNDERSCORE_BEFORE);

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_out, "EVE_OUTER_VID_SRC_AC_OUTER_VID", lif_outer_vid, "(LIF Outer VID)");
    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_out, "EVE_OUTER_VID_SRC_AC_INNER_VID", lif_inner_vid, "(LIF Inner VID)");

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_out, "EVE_OUTER_VID_SRC_ARP_AC_OUTER_VID", lif_outer_vid,
                                  "(LIF Outer VID)");
    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_out, "EVE_OUTER_VID_SRC_ARP_AC_INNER_VID", lif_inner_vid,
                                  "(LIF Inner VID)");

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_out, "EVE_OUTER_VID_SRC_FORWARD_DOMAIN", forward_domain,
                                  "(Forward_Domain)");

    __PRT_TABLE_ENTRY_PARSE_STR(2, "Outer_PCP_DEI_source", pcp_dei_src_out, pcp_dei_parse_out,
                                PRT_ROW_SEP_UNDERSCORE_BEFORE);

    /** Inner tag info */
    __PRT_TABLE_ENTRY_PARSE_INT(2, "Inner_TPID_index", tpid_idx_in, tpid_value_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);

    __PRT_TABLE_ENTRY_PARSE_STR(2, "Inner_VID_source", vid_src_in, vid_parse_in, PRT_ROW_SEP_UNDERSCORE_BEFORE);

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_in, "EVE_INNER_VID_SRC_AC_OUTER_VID", lif_outer_vid, "(LIF Outer VID)");
    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_in, "EVE_INNER_VID_SRC_AC_INNER_VID", lif_inner_vid, "(LIF Inner VID)");

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_in, "EVE_INNER_VID_SRC_ARP_AC_OUTER_VID", lif_outer_vid,
                                  "(LIF Outer VID)");
    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_in, "EVE_INNER_VID_SRC_ARP_AC_INNER_VID", lif_inner_vid,
                                  "(LIF Inner VID)");

    __CHECK_IF_SRC_LIF_PRINT_NAME(2, vid_parse_in, "EVE_INNER_VID_SRC_FORWARD_DOMAIN", forward_domain,
                                  "(Forward_Domain)");

    __PRT_TABLE_ENTRY_PARSE_STR(2, "Inner_PCP_DEI_source", pcp_dei_src_in, pcp_dei_parse_in,
                                PRT_ROW_SEP_UNDERSCORE_BEFORE);

    /** Different Field between FWD and ENC EVE*/
    if (!is_encap_stage)
    {
        __PRT_TABLE_ENTRY(2, "Tags_to_remove", eve_fwd_tags_to_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);

        __PRT_TABLE_ENTRY(0, "Before eve Num_of_tags", before_eve_num_of_tags, PRT_ROW_SEP_EQUAL_BEFORE);
        __PRT_TABLE_ENTRY(0, "Eve num_of_tags_to_add", eve_num_of_tags_to_add, PRT_ROW_SEP_UNDERSCORE_BEFORE);
    }
    else
    {
        __PRT_TABLE_ENTRY(2, "Post_eve_tx_tag_remove", post_eve_tx_tag_remove, PRT_ROW_SEP_UNDERSCORE_BEFORE);
        __PRT_TABLE_ENTRY(2, "Post_eve_vlan_membership", post_eve_vlan_membership, PRT_ROW_SEP_UNDERSCORE_BEFORE);
        __PRT_TABLE_ENTRY(2, "Post_eve_vlan_mirror", post_eve_vlan_mirror, PRT_ROW_SEP_UNDERSCORE_BEFORE);

        __PRT_TABLE_ENTRY(0, "Before eve Num_of_tags", before_eve_num_of_tags, PRT_ROW_SEP_EQUAL_BEFORE);
        __PRT_TABLE_ENTRY(0, "Eve num_of_tags_to_add", eve_num_of_tags_to_add, PRT_ROW_SEP_UNDERSCORE_BEFORE);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - main function of Egress VLAN editing information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_eve_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv;
    int core_in, core_id;
    char *eve_stage = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    PRT_INIT_VARS;

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);
    SH_SAND_GET_STR("stage", eve_stage);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        uint32 fwd_vlan_edit_profile, encap_vlan_edit_profile, vlan_edit_profile;
        uint32 etps_valid;
        uint8 etps_index;
        uint8 encap_ac_etps_id = 0, fwd_ac_etps_id = 0, ac_etps_id;
        char sig_name[RHNAME_MAX_SIZE];

        fwd_vlan_edit_profile = encap_vlan_edit_profile = EGRESS_VLAN_EDIT_PROFILE_INVALID;

        /** Get etps info */
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "ETPP", "Term", "FWD", "ETPS_Valid", &etps_valid, 1));

        /** Check if we have more than one AC entries*/
        for (etps_index = 1; etps_index < 9; etps_index++)
        {
            if (!((etps_valid >> (etps_index - 1)) && 0x1))
            {
                /** All possible ETPS were iterated already.*/
                break;
            }

            sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "ETPS.%d.VLAN_EDIT_PROFILE", etps_index);
            rv = dpp_dsig_read(unit, core_id, "ETPP", "Term", "FWD", sig_name, &vlan_edit_profile, 1);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else if (rv != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed in getting vlan_edit_profile from ETPS %d", etps_index + 1);
            }

            if (fwd_vlan_edit_profile == EGRESS_VLAN_EDIT_PROFILE_INVALID)
            {
                fwd_vlan_edit_profile = vlan_edit_profile;
                fwd_ac_etps_id = etps_index;
            }
            else
            {
                encap_vlan_edit_profile = vlan_edit_profile;
                encap_ac_etps_id = etps_index;

                /** Suppose only two AC entries at most: one for native AC, the other one for outer AC.*/
                break;
            }
        }

        /*
         * Get and print native EVE information with native AC
         */
        if (strcaseindex(eve_stage, "all") || strcaseindex(eve_stage, "NaTiVe") || strcaseindex(eve_stage, "ntv"))
        {
            /** Native EVE happen before outer EVE, maybe in FWD or ENC stage */
            if (encap_vlan_edit_profile == EGRESS_VLAN_EDIT_PROFILE_INVALID)
            {
                /** If only one AC entry, we think it is used in EVE (No Native EVE) */
                vlan_edit_profile = EGRESS_VLAN_EDIT_PROFILE_INVALID;
                ac_etps_id = 0;
            }
            else
            {
                /** If more than one AC entries, we think the first one is for Native EVE) */
                vlan_edit_profile = fwd_vlan_edit_profile;
                ac_etps_id = fwd_ac_etps_id;
            }
            SHR_IF_ERR_EXIT(sh_dnx_pp_eve_info_print(unit, core_id, vlan_edit_profile, ac_etps_id, TRUE, sand_control));
        }

        /*
         * Get and print outer EVE information with outer AC
         */
        if (strcaseindex(eve_stage, "all") || strcaseindex(eve_stage, "out"))
        {
            /** Outer EVE may be done in FWD stage or ENC stage.*/
            if (encap_vlan_edit_profile != EGRESS_VLAN_EDIT_PROFILE_INVALID)
            {
                vlan_edit_profile = encap_vlan_edit_profile;
                ac_etps_id = encap_ac_etps_id;
            }
            else
            {
                vlan_edit_profile = fwd_vlan_edit_profile;
                ac_etps_id = fwd_ac_etps_id;
            }
            SHR_IF_ERR_EXIT(sh_dnx_pp_eve_info_print
                            (unit, core_id, vlan_edit_profile, ac_etps_id, FALSE, sand_control));
        }
    } /** of for of core_id */

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_ive_man = {
    .brief = "PP VISibility IngressVlanEditing diagnostics",
    .full = "Shows the Ingress VLAN editing information of the last received packet",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "\n" "SHORT",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

sh_sand_option_t dnx_pp_ive_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};

sh_sand_man_t dnx_pp_eve_man = {
    .brief = "PP VISibility EgressVlanEditing diagnostics",
    .full = "Shows the Egress VLAN editing information of the last received packet",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "\n" "SHORT\n" "stage=NaTiVe\n" "stage=all",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

sh_sand_option_t dnx_pp_eve_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {"STAGE", SAL_FIELD_TYPE_STR, "Outer, NaTiVe or all", "Outer"}
    ,
    {NULL}
};
