/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_ppi.c
 *
 * Packet Parsing Information PP Diagnostics.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <bcm_int/dnx/instru/instru_visibility.h>
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

/*************
 * TYPEDEFS  *
 */

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */
/**
 * \brief - Diagnostic to see the packet parsing record
 *        that describes the header stack of network headers
 *        and their associated header qualifiers
 *        - Support SHORT mode which prints all but layer_qualifiers
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
sh_dnx_pp_ppi_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_in, core_id;

    /** signal element which contains the value of signal and Entry struct */
    signal_output_t *sig_list_element = NULL;
    signal_output_t *field_list_element = NULL;
    /** elements and sub fields of layer qualifier list*/
    signal_output_t *lq_list_element = NULL;
    signal_output_t *protocols_list_element = NULL;
    /** tag structure */
    signal_output_t *tag_s_list_element = NULL;
    /** layer offsets */
    signal_output_t *offsets_list_element = NULL;

    signal_output_t *sys_hdr_list_element = NULL;

    /** No need to free this one - contains field of actual signal*/
    signal_output_t *lq_sub_field_element;
    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        /** layer name and count */
        char name[RHNAME_MAX_SIZE] = "";
        int layer_count = 1;

        /** sub layer match counter, in which the index matches the sh_dnx_pp_sub_layer_names string location */
        int sub_layer_match_counter_arr[128] = { 0 };

        /** describes the top layer name, which is not sublayer to know when to print tag structure */
        char top_layer_name[RHNAME_MAX_SIZE] = "";

        char packet_structure_str[DSIG_MAX_SIZE_STR] = "";

        /** Port Information variables*/
#define MAX_PORT_SIGNALS        5
        int i_sig, nof_signals;
        bcm_instru_vis_signal_key_t signal_key[MAX_PORT_SIGNALS];
        bcm_instru_vis_signal_result_t signal_result[MAX_PORT_SIGNALS];
        char *metadata[MAX_PORT_SIGNALS];
        /*
         * This array defines the top layer names in order, so that retrieval of next protocol would be possible must
         * be created from scratch per Core, hence re-initialized in Core_Id scope
         */
        char next_protocol_array[SH_DNX_PP_MAX_LAYERS_NUMBER][RHNAME_MAX_SIZE] = { "" };
        int next_protocol_idx = 0;

        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, !short_mode);

        /** get the layer qualifier */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "", "layer_qualifiers",
                                                    &lq_list_element), "no signal record for layer_qualifiers\n");

        /** get the layer protocols */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "", "layer_protocols",
                                                    &protocols_list_element), "no signal record for layer_protocols\n");

        /** get the layer offsets */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "", "layer_offsets",
                                                    &offsets_list_element), "no signal record for layer_offsets\n");

        /** get the layer and its parsing */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "NIF", "", "header", &sig_list_element),
                            "no signal record for header from NIF\n");

        /** get tag structure value in Ingress*/
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT1", "", "Incoming_Tag_Structure",
                                                    &tag_s_list_element),
                            "no signal record for Incoming_Tag_Structure\n");

        /** don't show PP PPI in short mode, but 'PP VIS LAST' */
        if (!short_mode)
        {
            PRT_TITLE_SET("Packet_Parsing and Port Information, Core=%d", core_id);
        }
        else
        {
            PRT_TITLE_SET("IRPP Last Packet Information, Core=%d", core_id);
        }

        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size (bits)");

        /** get the overall packet structure and return the packet structure string + update layer linked list for 'next layer' info */
        SHR_IF_ERR_EXIT(construct_packet_structure
                        (unit, sig_list_element->field_list, packet_structure_str, next_protocol_array, NULL));

        PRT_INFO_ADD("Packet Structure: %s", packet_structure_str);

        /*
         * 'sig_list_element' contains the complete packet header - print it
         */
        sal_strncpy(name, SH_DNX_PP_GET_SIG_EXP_NAME(sig_list_element), RHNAME_MAX_SIZE - 1);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("0x%s", sig_list_element->print_value);
        PRT_CELL_SET("%3db", sig_list_element->size);
        PRT_COMMITX;

        PRT_TITLE_SET("Port Information");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size (bits)");

        /*
         * Print Port Information Table
         */
        i_sig = 0;
        sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
        sal_strncpy(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].signal, "TM_Cmd.in_pp_port", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        metadata[i_sig] = "In_PP_Port";

        i_sig++;
        sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
        sal_strncpy(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].signal, "Src_Sys_Port_ID", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        metadata[i_sig] = "Source_System_Port";

        i_sig++;
        sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
        sal_strncpy(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].signal, "PTC", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        metadata[i_sig] = "In_TM_Port";

        i_sig++;
        sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
        sal_strncpy(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].from, "LLR", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key[i_sig].signal, "Prt_Qualifier", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        metadata[i_sig] = "Packet_Qualifier";

#ifndef ADAPTER_SERVER_MODE
        if (dnx_data_debug.feature.feature_get(unit, dnx_data_debug_feature_iparser_signals_valid))
        {
            i_sig++;
            sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
            sal_strncpy(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[i_sig].from, "IParser", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
            sal_strncpy(signal_key[i_sig].signal, "Parsing_Context", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
            metadata[i_sig] = "PP_Context";
        }
#endif

        nof_signals = i_sig + 1;
        SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

        for (i_sig = 0; i_sig < nof_signals; i_sig++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%s", metadata[i_sig]);
            if (signal_result[i_sig].status == BCM_E_NONE)
            {
                PRT_CELL_SET("0x%x", signal_result[i_sig].value[0]);
                PRT_CELL_SET("%3d", signal_result[i_sig].size);
            }
            else
            {
                PRT_CELL_SET("Not Found");
            }
        }

        /** close table */
        PRT_COMMITX;

        /** set the layer qualifier list to first sub field (first layer qualifier) */
        lq_sub_field_element = utilex_rhlist_entry_get_first(lq_list_element->field_list);

        /** set the layer protocols list to first sub field (first layer protocol) */
        protocols_list_element = utilex_rhlist_entry_get_first(protocols_list_element->field_list);

        /** set the layer offset list to first sub field (first layer offset) */
        offsets_list_element = utilex_rhlist_entry_get_first(offsets_list_element->field_list);

        /*
         * Traverse from top field to the last and print its name and value this should be done recursively since there
         * may be unknown hierarchies of info under each the below outmost iteration would be on the layers for parsing
         * we need to iterate the 'sig_list_element->field_list' recursively
         */

        /*
         *  start the next protocol index to 1 (first next protocol) as we iterate through the layers
         *  we increase the index each time we encounter a non sub-layer layer
         */

        next_protocol_idx = 1;

        if (!short_mode)
        {

            RHITERATOR(field_list_element, sig_list_element->field_list)
            {
                /** current layer name */
                char *name = SH_DNX_PP_GET_SIG_EXP_NAME(field_list_element);
                char print_value[DSIG_MAX_SIZE_STR] = "";

                /** define next layer pointer and layer name for knowing when to close the table */
                signal_output_t *next_field_list_element = utilex_rhlist_entry_get_next(field_list_element);
                char next_layer_name[RHNAME_MAX_SIZE] = "";
                uint8 next_sub_layer_flag = 0;

                uint8 system_header_flag = 0;

                /** Array which corresponds to system headers and their specific command to get layer qualifier, "" means no print */
                char *system_header_qualifiers_commands[] =
                    { "prt_qualifier", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };
                /** the location of system header in the array that matched the layer name */
                int system_hdr_idx = 0;

                uint8 sub_layer_flag = 0;
                /** the location of sublayer in the array that matched the layer name */
                int sub_layer_idx = 0;

                /** This array contains layers for which the qualifier should not be presented */
                char *skip_qualifier_layers[] = { "MPLS", "" };
                /** corresponds to the skip qualifier layer names, represents how many records in layer qualifier to skip for this layer */
                int skip_qualifier_num_of_skips[] = { 2, 0 };
                uint8 skip_qualifier_layer_flag = 0;
                /** the location of layer in the array that matched the layer name */
                int skip_ql_idx = 0;

                /** initial shift count for sub-fields of layers*/
                int shifts = 1;

                /** for checking if str is a number */

                /*
                 * scan through non layer strings to see if name is one of them
                 */

                system_hdr_idx = string_in_array(name, FALSE, dnx_pp_system_header_names);
                if (system_hdr_idx != -1)
                {
                    system_header_flag = 1;
                }

                /*
                 * scan through sub layer strings to see if name is one of them and save the match location (to use as
                 * print for example VLAN 1, VLAN 2..) and update the string match counter in sub_layer_match_counter_arr
                 */

                sub_layer_idx = string_in_array(name, FALSE, sh_dnx_pp_sub_layer_names);
                if (sub_layer_idx != -1)
                {
                    sub_layer_flag = 1;
                    sub_layer_match_counter_arr[sub_layer_idx]++;
                }

                /*
                 * Scan if the layer is one of layers for which not to present the qualifier information
                 */

                skip_ql_idx = string_in_array(name, FALSE, skip_qualifier_layers);

                if (skip_ql_idx != -1)
                {
                    skip_qualifier_layer_flag = 1;
                }

                sal_strncat(print_value, field_list_element->print_value, DSIG_MAX_SIZE_STR - 1);

                /*
                 *  decide how the print would look like per each layer (or sysheader)/sublayer
                 */

                if (sub_layer_flag)
                {
                    /*
                     * in case the name is a sublayer, print with a shift and set for its sub-fileds to 2 shifts also print
                     * the order of the sub layer - for example - VALN1, VLAN2...
                     */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET_SHIFT(1, "%s_%d", name, sub_layer_match_counter_arr[sub_layer_idx]);
                    shifts = 2;
                    PRT_CELL_SET("%s", print_value);
                    PRT_CELL_SET("%3d", field_list_element->size);

                }
                else
                {
                    /*
                     * regular layer/system header, print with layer count, and start a new table + next protocol
                     */

                    PRT_TITLE_SET("Header %d %s", layer_count, name);
                    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
                    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
                    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size (bits)");

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("Packet Header");
                    PRT_CELL_SET("%s", print_value);
                    PRT_CELL_SET("%3d", field_list_element->size);

                    layer_count++;

                    /** update the top layer name only if it's a real layer - later for tag structure print decision */
                    sal_strncpy(top_layer_name, name, RHNAME_MAX_SIZE - 1);

                    /** zero all sub-fields counters at each new layer to not drag say VLAN count from one ETH to next */
                    sal_memset(sub_layer_match_counter_arr, 0, sizeof sub_layer_match_counter_arr);

                }

                /************************** PARSING PRINT ********************************************
                 * call recursive function to continue printing down the hierarchy of the layer record
                 * in order to print to same table passing the 'prt_ctr' controller which was created
                 * in PRT_INIT_VARS
                 */

                SHR_IF_ERR_EXIT(recursive_field_print_iterator
                                (unit, field_list_element->field_list, prt_ctr, shifts, NULL));

                /*
                 *  close the table only if next layer is non a sub-layer such as VLAN
                 */

                /** update the name of next layer only if the pointer to next layer is non NULL */
                if (next_field_list_element != NULL)
                {
                    sal_strncpy(next_layer_name, SH_DNX_PP_GET_SIG_EXP_NAME(next_field_list_element),
                                RHNAME_MAX_SIZE - 1);

                    /** scan through sub layer strings to see if next layer name is one of them */
                    if (string_in_array(next_layer_name, FALSE, sh_dnx_pp_sub_layer_names) != -1)
                    {
                        next_sub_layer_flag = 1;
                    }

                }

                /** only if next layer is non a sublayer - i.e about to close the table, print tag struct and qualifiers */

                if (!next_sub_layer_flag)
                {

                    if (!system_header_flag)
                    {

                        /******************************* TABLE INFO PRINT **************************************************
                         *
                         * If next layer is not sub-layer or system header, add the "info" section in the table - layer offset,
                         * and if ETH - also tag_s and initial_vid
 */

                        /** print layer offset */
                        PRT_INFO_ADD("Layer offset 0x%s", offsets_list_element->print_value);

                        /** print next protocol */
                        if (next_protocol_array[next_protocol_idx][0] != '\0')
                        {
                            PRT_INFO_ADD("Next Protocol %s", next_protocol_array[next_protocol_idx]);
                            PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
                        }

                        /*
                         *******************  TAG STRUCTURE & INITIAL VID PRINT (for top layer ETH only) ****************
                         */
                        if (sal_strcasestr(top_layer_name, "ETH"))
                        {

                            /** tag structure info variables */
                            char tag_s_ing_name[RHNAME_MAX_SIZE] = "";
                            char tag_s_ing_print_value[DSIG_MAX_SIZE_STR] = "";
                            uint32 tag_s_ing_value_hw;
                            char tag_s_field_name[DBAL_MAX_STRING_LENGTH] = "";
                            char tag_s_ing_parse[DBAL_MAX_STRING_LENGTH] = "";

                            char *trimmed_str;

                            bcm_port_tag_format_class_t tag_s_ing_value_sw;

                            /** initial VID variables */
                            uint32 vid_list_element;

                            /** is outer tpid priority variables */
                            uint32 tpid_prio_list_element;

                            int tpid_idx;

                            int rv;

                            /*
                             * get the s-tag structure according to its value from DBAL_FIELD_INCOMING_TAG_STRUCTURE ENUMs
                             * (value --> 's-tag/c-tag..etc.') translate the ENUM field to actual string
                             */

                            sal_strncpy(tag_s_ing_name, SH_DNX_PP_GET_SIG_EXP_NAME(tag_s_list_element),
                                        RHNAME_MAX_SIZE - 1);
                            sal_strncpy(tag_s_ing_print_value, tag_s_list_element->print_value, RHNAME_MAX_SIZE - 1);
                            tag_s_ing_value_hw = tag_s_list_element->value[0];

                            sal_strncpy(tag_s_field_name, dbal_field_to_string(unit, DBAL_FIELD_INCOMING_TAG_STRUCTURE),
                                        RHNAME_MAX_SIZE - 1);
                            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                                            (unit, tag_s_field_name, tag_s_ing_value_hw, tag_s_ing_parse));

                            /** increase the string pointer by length of 'INCOMING_TAG_STRUCTURES_' to get only the 'S_TAG/C_TAG...' */
                            trimmed_str = &tag_s_ing_parse[sal_strlen(tag_s_field_name) + 1];

                            /** translate hw id to sw id */
                            dnx_port_tpid_class_hw_id_get_info(unit, TRUE, (int) tag_s_ing_value_hw,
                                                               &tag_s_ing_value_sw);

                            PRT_INFO_ADD("%s hw value 0x%s (%s) / sw value (0x%d)", tag_s_ing_name,
                                         tag_s_ing_print_value, trimmed_str, tag_s_ing_value_sw);
                            PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);

                            /*
                             * Get the Initial VID and Print
                             */
                            SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP", "VTT4", "", "initial_vid",
                                                              &vid_list_element, 1),
                                                "no signal record for initial_vid, please run a packet !\n");

                            PRT_INFO_ADD("Initial VID 0x%x", vid_list_element);
                            PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);

                            /*
                             * Get the ETH Layer Qualifier TPID Indexes and transform to Global TPID Indexes (for the DBAL table)
                             * then get the DBAL value and print in Table Info
                             */
                            for (tpid_idx = 1; tpid_idx < 4; tpid_idx++)
                            {
                                char tpid_exists_str[RHNAME_MAX_SIZE];
                                char tpid_index_str[RHNAME_MAX_SIZE];

                                signal_output_t *tpid_exists_handle;
                                signal_output_t *tpid_index_handle;

                                 /** switch between which tpid to read */
                                switch (tpid_idx)
                                {
                                    case 1:
                                        sal_strncpy(tpid_exists_str, "1st_tpid_exist", RHNAME_MAX_SIZE - 1);
                                        sal_strncpy(tpid_index_str, "1st_tpid_index", RHNAME_MAX_SIZE - 1);
                                        break;
                                    case 2:
                                        sal_strncpy(tpid_exists_str, "2nd_tpid_exist", RHNAME_MAX_SIZE - 1);
                                        sal_strncpy(tpid_index_str, "2nd_tpid_index", RHNAME_MAX_SIZE - 1);
                                        break;
                                    case 3:
                                        sal_strncpy(tpid_exists_str, "3rd_tpid_exist", RHNAME_MAX_SIZE - 1);
                                        sal_strncpy(tpid_index_str, "3rd_tpid_index", RHNAME_MAX_SIZE - 1);
                                        break;
                                    default:
                                        break;
                                }

                                tpid_exists_handle =
                                    (signal_output_t *)
                                    utilex_rhlist_entry_get_by_name(lq_sub_field_element->field_list, tpid_exists_str);
                                tpid_index_handle =
                                    (signal_output_t *)
                                    utilex_rhlist_entry_get_by_name(lq_sub_field_element->field_list, tpid_index_str);

                                 /** if the tpid is marked as exists and its not NULL (in case of non existing field)*/
                                if (tpid_exists_handle != NULL && tpid_exists_handle->value[0] != 0)
                                {
                                    /*
                                     * Ingress TPID conversion:
                                     * Ethernet-Layer-Qualifier TPID index to Global TPID index :
                                     */
                                    const int
                                        ingress_qual_to_global_tpid_index_convert[BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS] =
                                        { 7, 0, 1, 2, 3, 4, 5, 6 };

                                     /** convert the qualifier tpid to global tpid index for DBAL access */
                                    int tpid_global_idx =
                                        ingress_qual_to_global_tpid_index_convert[tpid_index_handle->value[0]];

                                    uint16 tpid_value;

                                    char tpid_type_str[RHNAME_MAX_SIZE];

                                     /** get the tpid value from DBAL */
                                    dnx_switch_tpid_value_get(unit, tpid_global_idx, &tpid_value);

                                     /** mark the tpid as inner or outer */
                                    if (tpid_idx == 1)
                                    {
                                        sal_strncpy(tpid_type_str, "Outer TPID", RHNAME_MAX_SIZE - 1);
                                    }
                                    else
                                    {
                                        sal_strncpy(tpid_type_str, "Inner TPID", RHNAME_MAX_SIZE - 1);
                                    }

                                     /** print this tpid */
                                    PRT_INFO_ADD("%s 0x%x", tpid_type_str, tpid_value);
                                    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);

                                }  /** of if tpid exists */

                            }  /** of for tpid_idx */

                            /*
                             * Get the Is Outer tpid Priority
                             */
                            rv = dpp_dsig_read(unit, core_id, "ERPP", "PRP", "",
                                               "eth_tag_format.outer_tag_is_priority", &tpid_prio_list_element, 1);

                            /** show this ERPP signal, only if packet passed to Egress and signal exists */
                            if (rv == _SHR_E_NONE)
                            {
                                PRT_INFO_ADD("Outer Tag is Priority 0x%x", tpid_prio_list_element);
                                PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
                            }

                        } /** of if ETH */

                        /*
                         ************************* LAYER QUALIFIER PRINT + LAYER OFFSET POINTER FORWARDING **********************
                         */

                        /** do the print only if the protocol list is non zero & the layer is not defined in skip qualifier array */
                        if (protocols_list_element->value[0] != 0)
                        {

                            if (!skip_qualifier_layer_flag)
                            {

                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                                /** print the name of the layer qualifier */
                                PRT_CELL_SET("%s", SH_DNX_PP_GET_SIG_EXP_NAME(lq_sub_field_element));
                                PRT_CELL_SKIP(2);

                                /** print recursively the down hierarchy for this layer qualifier */
                                SHR_IF_ERR_EXIT(recursive_field_print_iterator
                                                (unit, lq_sub_field_element->field_list, prt_ctr, 1, NULL));

                                /*
                                 *  also move next layer in qualifier, protocols and offset list, which corresponds to real layer names in 'sig_list'
                                 */
                                lq_sub_field_element = utilex_rhlist_entry_get_next(lq_sub_field_element);
                                protocols_list_element = utilex_rhlist_entry_get_next(protocols_list_element);
                                offsets_list_element = utilex_rhlist_entry_get_next(offsets_list_element);

                            }
                            else if (sal_strcmp(next_layer_name, skip_qualifier_layers[skip_ql_idx]) != -0)
                            {
                                /*
                                 * -in this case it's a layer for which to skip the qualifier, and we skip as defined for this layer in the nof skips array
                                 * -we check that next layer name is different from current skip layer name, to make sure we skip only at the end of skip layer group
                                 */

                                int q;
                                for (q = 0; q < skip_qualifier_num_of_skips[skip_ql_idx]; q++)
                                {
                                    lq_sub_field_element = utilex_rhlist_entry_get_next(lq_sub_field_element);
                                    protocols_list_element = utilex_rhlist_entry_get_next(protocols_list_element);
                                    offsets_list_element = utilex_rhlist_entry_get_next(offsets_list_element);
                                }

                            }
                        } /** of protocols_list_element->field_list != NULL */

                    }
                    else
                    {
                        /*
                         ************************* SYSTEM LAYER QUALIFIER PRINT ************************
                         * In case of a system layer, check the system's layer command for its qualifier
                         */

                        /** check that the qualifier command for sysheader is not empty string & the layer not defined as skip qualifier */
                        if ((sal_strcmp(system_header_qualifiers_commands[system_hdr_idx], "") != -0)
                            && (!skip_qualifier_layer_flag))
                        {
                            SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "",
                                                                    system_header_qualifiers_commands[system_hdr_idx],
                                                                    &sys_hdr_list_element));
                            if (sys_hdr_list_element == NULL)
                            {
                                SHR_ERR_EXIT(_SHR_E_NONE, "no signal record for %s, please run a packet !\n",
                                             system_header_qualifiers_commands[system_hdr_idx]);
                            }

                            if (sys_hdr_list_element == NULL)
                            {
                                SHR_ERR_EXIT(_SHR_E_NONE, "no signal record for %s, please run a packet !\n",
                                             system_header_qualifiers_commands[system_hdr_idx]);
                            }

                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                            PRT_CELL_SET("%s", SH_DNX_PP_GET_SIG_EXP_NAME(sys_hdr_list_element));
                            PRT_CELL_SET("%s", sys_hdr_list_element->print_value);
                            PRT_CELL_SET("%3d", sys_hdr_list_element->size);

                        }

                        /** Print next protocol for system header - with line separation */
                        if (next_protocol_array[next_protocol_idx][0] != '\0')
                        {
                            PRT_INFO_ADD("Next Protocol %s", next_protocol_array[next_protocol_idx]);
                            PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
                        }

                    } /** of if not system_header_flag */

                    /** increase the next protocol array index in case not syb layer flag - where we don't print next protocol */
                    next_protocol_idx++;

                    PRT_COMMITX;

                } /** of if not next_sub_layer_flag */

            } /** of ITERATOR */

        } /** of if short_mode */

    } /** of core id for loop */

exit:
    sand_signal_output_free(sig_list_element);
    sand_signal_output_free(lq_list_element);
    sand_signal_output_free(protocols_list_element);
    sand_signal_output_free(tag_s_list_element);
    sand_signal_output_free(offsets_list_element);
    sand_signal_output_free(sys_hdr_list_element);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_ppi_man = {
    .brief = "Packet Parsing and Port Information",
    .full = "Result of the packet parsing record and port information.\n"
        "Presents the stack of network headers and their associated qualifiers.\n"
        "Use core= argument to have it per core",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "\n" "SHORT"
};

sh_sand_option_t dnx_pp_ppi_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
