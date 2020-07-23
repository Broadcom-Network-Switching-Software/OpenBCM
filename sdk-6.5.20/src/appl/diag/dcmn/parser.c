/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        parser.c
 * Purpose:     Diag shell parser commands
 */

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <shared/bsl.h>

#include <sal/appl/sal.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dpp/JER/jer_parser.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/drv.h>
/*
 * General shell style usage
 */
const char cmd_dpp_parser_usage[] = "Please use \"parser(prs) usage\" for advanced help\n";

sh_sand_man_t sh_dpp_parser_man = {
    "Present available parser resources",
    NULL,
    NULL,
    NULL,
};

sh_sand_man_t parser_segment_man = {
   "Present allocated parser segments",
   "Command presents allocated parser segments",
   NULL,
   NULL
};

sh_sand_man_t parser_macro_man = {
   "Present allocated macro segments",
   "Command presents allocated dynamic macros",
   NULL,
   NULL
};

static sh_sand_option_t parser_segment_options[] = {
    {"name",          SAL_FIELD_TYPE_STR,  "Segment Name or Name substring",                      ""},
    {"id",            SAL_FIELD_TYPE_INT32,"Segment Allocated Base Number(0-31)",                 "-1"},
    {"entry",         SAL_FIELD_TYPE_BOOL, "Show entries of the segment chosen",                  "No"},
    {"all",           SAL_FIELD_TYPE_BOOL, "Show all segments with all entries",                  "No"},
    {NULL}
};

static shr_error_e
print_segment_leaf_set(
        int unit,
        int first,
        int last,
        uint32 macro,
        uint32 base,
        uint32 pfc,
        uint32 plc,
        prt_control_t *prt_ctr)
{
    char *next_segment_name;

    SHR_FUNC_INIT_VARS(unit);

    if(first == last)
    {
        PRT_CELL_SET("Entry:%d", first);
    }
    else
    {
        PRT_CELL_SET("Entry:%d-%d", first, last);
    }

    if(base == JER_SEGMENT_END)
    {
        PRT_CELL_SET("NA");
    }
    else
    {
        /* Some patching to have proper recognition of TM macros, no changes are expected, so... */
        int tm_flag = FALSE;
        if((pfc >= ARAD_PARSER_PFC_RAW_AND_FTMH) || (jer_parser_segment_id_by_base(unit, base) == DPP_SEGMENT_TM))
            tm_flag = TRUE;

        PRT_CELL_SET("%s(%d)", jer_parser_macro_string_by_hw(unit, macro, tm_flag), macro);
    }
    if ((SOC_DPP_CONFIG(unit)->pp.parser_mode == 1) || (base == JER_SEGMENT_FIN) || (base == JER_SEGMENT_END))
        next_segment_name = jer_parser_segment_string_by_base(unit, base);
    else
        next_segment_name = parser_segment_name[base >> 2];

    PRT_CELL_SET("%s(0x%x)", next_segment_name, base);
    PRT_CELL_SET("%s(0x%x)", dpp_parser_pfc_string_by_hw(unit, pfc), pfc);
    PRT_CELL_SET("%s(0x%x)", dpp_parser_plc_string_by_hw(unit, pfc, plc), plc);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
print_segment_content(
    int unit,
    char *name,
    uint32 base,
    int segment_size,
    int entry_flag,
    char *match_name,
    int match_base,
    prt_control_t *prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);

    if(match_base == -1)
    { /* For no ID check name filter */
        if(!ISEMPTY(match_name) && !sal_strcasestr(name, match_name))
            SHR_EXIT();
    }
    else if(base != match_base)
    {
        SHR_EXIT();
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", name);
    if(segment_size != JER_SEGMENT_FULL_SIZE)
    {
        PRT_CELL_SET("%d.%d", base >> 2, base & JER_SEGMENT_COMPLEX_MASK);
    }
    else
    {
        PRT_CELL_SET("%d", base >> 2);
    }
    if(entry_flag == TRUE)
    {
        int i_entry, last = 0, first = 0;
        uint32 cur_base, cur_macro, cur_pfc, cur_plc;
        uint32 prev_base = 0, prev_macro = 0, prev_pfc = 0, prev_plc = 0;
        ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  tbl_data;
        ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA packet_format;

        for(i_entry = 0; i_entry < segment_size; i_entry++) {
            arad_pp_ihp_parser_program_tbl_get_unsafe(unit, 0, base * JER_SEGMENT_SMALL_SIZE + i_entry, &tbl_data);
            arad_pp_ihp_packet_format_table_tbl_get_unsafe(unit, base * JER_SEGMENT_SMALL_SIZE + i_entry, &packet_format);

            cur_pfc   = packet_format.packet_format_code;
            cur_plc   = packet_format.parser_leaf_context;
            cur_base  = tbl_data.next_addr_base;
            cur_macro = tbl_data.macro_sel;
            if(i_entry == 0) {
                /* First Element in segment - never printing waiting for next */
                last = first = i_entry;
                prev_base  = cur_base;
                prev_macro = cur_macro;
                prev_pfc   = cur_pfc;
                prev_plc   = cur_plc;
            }
            else if((prev_base == cur_base) && (prev_macro == cur_macro) && (prev_pfc == cur_pfc) && (prev_plc == cur_plc))
            {
                /* Next element in chains of equal, not printing waiting for next, unless last in segment */
                last = i_entry;
            }
            else {
                /* Different element, so we need to print previous one */
                if(first != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(2);
                }
                SHR_IF_ERR_EXIT(print_segment_leaf_set(unit, first, last, prev_macro, prev_base, prev_pfc, prev_plc, prt_ctr));
                /* And start new one counting */
                last = first = i_entry;
                prev_base  = cur_base;
                prev_macro = cur_macro;
                prev_pfc   = cur_pfc;
                prev_plc   = cur_plc;
            }
            if(i_entry == (segment_size - 1))
            {
                if(first != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(2);
                }
                SHR_IF_ERR_EXIT(print_segment_leaf_set(unit, first, last, cur_macro, cur_base, cur_pfc, cur_plc, prt_ctr));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
parser_segment_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_name;
    int i_sgm, match_base;
    int entry_flag, all_flag;
    uint32 base;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", match_name);
    SH_SAND_GET_INT32("id", match_base);
    SH_SAND_GET_BOOL("entry", entry_flag);
    SH_SAND_GET_BOOL("all", all_flag);

    if(all_flag == TRUE)
    {
        match_base = -1;
        entry_flag = TRUE;
        match_name = NULL;
    }
    PRT_TITLE_SET("Parser Segments List");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("ID");
    if(entry_flag == TRUE)
    {
        PRT_COLUMN_ADD("Entries");
        PRT_COLUMN_ADD("Macro");
        PRT_COLUMN_ADD("Segment");
        PRT_COLUMN_ADD("PFC");
        PRT_COLUMN_ADD("PLC");
    }

    for (i_sgm = 0; i_sgm < JER_SEGMENT_NOF; i_sgm++)
    {
        char *cur_segment_name;
        DPP_SEGMENT_E cur_segment_id;

        if (SOC_DPP_CONFIG(unit)->pp.parser_mode == 1)
        {
            if((cur_segment_id = segment_map[unit][i_sgm]) == DPP_SEGMENT_NA)
                continue;

            if(cur_segment_id == DPP_SEGMENT_COMPLEX)
            {
                int k_sgm;
                for(k_sgm = 0; k_sgm < JER_SEGMENT_COMPLEX_NOF; k_sgm++)
                {
                    int size;
                    cur_segment_id = segment_complex_map[unit][i_sgm][k_sgm];
                    if((cur_segment_id == DPP_SEGMENT_NA) || (cur_segment_id == DPP_SEGMENT_NOF))
                        continue;
                    base = (i_sgm << 2) | k_sgm;
                    cur_segment_name = jer_parser_segment_string_by_base(unit, base);
                    size = jer_parser_segment_size_by_sw(cur_segment_id);
                    print_segment_content(unit, cur_segment_name, base, size, entry_flag, match_name, match_base, prt_ctr);
                }
            }
            else
            {
                base = (i_sgm << 2);
                cur_segment_name = jer_parser_segment_string_by_base(unit, base);
                print_segment_content(unit, cur_segment_name, base, JER_SEGMENT_FULL_SIZE, entry_flag, match_name, match_base, prt_ctr);
            }
        }
        else
        {
            base = (i_sgm << 2);
            cur_segment_name = parser_segment_name[i_sgm];
            print_segment_content(unit, cur_segment_name, base, JER_SEGMENT_FULL_SIZE, entry_flag, match_name, match_base, prt_ctr);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t parser_macro_options[] = {
    {"id",            SAL_FIELD_TYPE_INT32,"Macro Allocated Number(0-7)",                         "-1"},
    {"entry",         SAL_FIELD_TYPE_BOOL, "Show content of the macro",                           "No"},
    {"all",           SAL_FIELD_TYPE_BOOL, "Show all macros with content",                        "No"},
    {NULL}
};

static char *comparator_mode[] =
{
    [0] = "Reserved-0",
    [1] = "16LSBTo16LSB",
    [2] = "16LSBTo16MSB",
    [3] = "32-Bits",
    [4] = "16LSBToEth",
    [5] = "8LSBToIP",
    [6] = "Reserved-6",
    [7] = "Reserved-7",
};

static shr_error_e
parser_macro_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_mcr;
    DPP_MACRO_E macro_id;
    int hw_id;
    int entry_flag, all_flag;
    char *macro_name;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", hw_id);
    SH_SAND_GET_BOOL("entry", entry_flag);
    SH_SAND_GET_BOOL("all", all_flag);

    if(all_flag == TRUE)
    {
        hw_id = -1;
        entry_flag = TRUE;
    }

    PRT_TITLE_SET("Parser Dynamic Macro List");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("ID");
    if(entry_flag)
    {
        PRT_COLUMN_ADD("WordOffset");
        PRT_COLUMN_ADD("LSBMasked");
        PRT_COLUMN_ADD("MSBMasked");
        PRT_COLUMN_ADD("ShiftSel");
        PRT_COLUMN_ADD("HdrSizeA");
        PRT_COLUMN_ADD("HdrSizeB");
        PRT_COLUMN_ADD("CmpMask");
        PRT_COLUMN_ADD("CmpMode");
        PRT_COLUMN_ADD("CondSel");
        PRT_COLUMN_ADD("HdrQualMask");
        PRT_COLUMN_ADD("AugmPrev");
        PRT_COLUMN_ADD("QualShift");
        PRT_COLUMN_ADD("QualOrMask");
        PRT_COLUMN_ADD("QualAndMask");
    }
    for (i_mcr = 0; i_mcr < JER_CUSTOM_MACRO_NOF * 2; i_mcr++)
    {
        if((hw_id != -1) && (i_mcr != hw_id))
            continue;

        /* For TM Macro we may use Jericho facility */
        if(i_mcr >= JER_CUSTOM_MACRO_NOF)
        {
            macro_name = jer_parser_macro_string_by_hw(unit, i_mcr - JER_CUSTOM_MACRO_NOF, TRUE);
        }
        else
        {
            if (SOC_DPP_CONFIG(unit)->pp.parser_mode == 1)
            {
                macro_id = macro_map[unit][i_mcr];
                if(macro_id == DPP_MACRO_NA)
                    continue;
                macro_name = jer_parser_macro_string_by_sw(macro_id);
            }
            else
            {
                macro_name = arad_parser_get_macro_str(unit, i_mcr);
            }
        }
        if(ISEMPTY(macro_name))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", macro_name);
        PRT_CELL_SET("%d", i_mcr);
        if(entry_flag)
        {
            ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA custom_macro_parameters;

            arad_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(unit, i_mcr, &custom_macro_parameters);
            PRT_CELL_SET("%d", custom_macro_parameters.cstm_word_select); /* bytes from the start of header - word 32bit */
            PRT_CELL_SET("%d", custom_macro_parameters.cstm_mask_right);  /* first LSB bits of word are masked */
            PRT_CELL_SET("%d", custom_macro_parameters.cstm_mask_left);   /* first MSB bits of word are masked */
            PRT_CELL_SET("%d", custom_macro_parameters.cstm_shift_sel);   /* Boolean indication approach to choose header size */
            PRT_CELL_SET("%x", custom_macro_parameters.cstm_shift_a);     /* Use in creating Header Size for the stage */
            PRT_CELL_SET("%x", custom_macro_parameters.cstm_shift_b);     /* Use in creating Header Size for the stage */
            PRT_CELL_SET("%x", custom_macro_parameters.cstm_comparator_mask); /* Each bit indicates if specific comparator participates in comparison */
            PRT_CELL_SET("%s", comparator_mode[custom_macro_parameters.cstm_comparator_mode]); /* Mode which comparator table choose */
            PRT_CELL_SET("%x", custom_macro_parameters.cstm_condition_select);  /* Used when Shift Select is Zero to choose Header Size */
            PRT_CELL_SET("%x", custom_macro_parameters.header_qualifier_mask);  /* relevant for augmented stage */
            PRT_CELL_SET("%s", custom_macro_parameters.augment_previous_stage ? "Yes" : "No"); /* Work on the same qualifier */
            PRT_CELL_SET("%x", custom_macro_parameters.qualifier_shift);        /* Used in creating qualifier */
            PRT_CELL_SET("%x", custom_macro_parameters.qualifier_or_mask);      /* Used in creating qualifier */
            PRT_CELL_SET("%x", custom_macro_parameters.qualifier_and_mask);     /* Used in creating qualifier */
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_pfc_man = {
   "Present activated pfc",
   "Command presents activated pfc",
   NULL,
   NULL,
};

static shr_error_e
parser_pfc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dpp_parser_pfc_info_t *pfc_info;
    int i_pfc;
    DPP_PFC_E pfc_sw;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parser Format Code");

    PRT_COLUMN_ADD("PFC");
    PRT_COLUMN_ADD("SW");
    PRT_COLUMN_ADD("HW");
    PRT_COLUMN_ADD("VTT");
    PRT_COLUMN_ADD("PMF");
    PRT_COLUMN_ADD("PROTOS");
    PRT_COLUMN_ADD("PLC Profile");

    for (i_pfc = 0; i_pfc < ARAD_PARSER_PFC_NOF; i_pfc++)
    {
        char proto_str[PRT_COLUMN_WIDTH];
        SHR_IF_ERR_EXIT(dpp_parser_pfc_get_sw_by_hw(unit, i_pfc, &pfc_sw));

        if(pfc_sw == DPP_PFC_NA)
            continue;

        pfc_info = parser_pfc_info_get_by_sw(unit, pfc_sw);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", pfc_info->name);
        PRT_CELL_SET("0x%02X", pfc_info->sw);
        PRT_CELL_SET("0x%02X", pfc_info->hw);

        PRT_CELL_SET("0x%02X", pfc_info->vtt);
        PRT_CELL_SET("0x%02X", pfc_info->pmf);

        SET_EMPTY(proto_str);
        if(pfc_info->is_proto & IS_ETH)
            utilex_str_append(proto_str, "eth");
        if(pfc_info->is_proto & IS_IPV4)
            utilex_str_append(proto_str, "ipv4");
        if(pfc_info->is_proto & IS_IPV6)
            utilex_str_append(proto_str, "ipv6");
        if(pfc_info->is_proto & IS_MPLS)
            utilex_str_append(proto_str, "mpls");
        if(pfc_info->is_proto & IS_TRILL)
            utilex_str_append(proto_str, "trill");
        if(pfc_info->is_proto & IS_UD)
            utilex_str_append(proto_str, "user defined");

        PRT_CELL_SET("%s", proto_str);
        PRT_CELL_SET("%s", dpp_parser_plc_profile_string_by_sw(unit, pfc_info->sw));
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_plc_man = {
   "Present plc profiles",
   "Command presents all available plc profiles",
   NULL,
   NULL
};

static shr_error_e
parser_plc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_prf, i_plc;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Packet Leaf Context Profiles");

    PRT_COLUMN_ADD("Profile");
    PRT_COLUMN_ADD("PLC");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("SW");
    PRT_COLUMN_ADD("HW");
    PRT_COLUMN_ADD("MASK");

    for (i_prf = 0; i_prf < dpp_parser_plc_profile_nof; i_prf++)
    {
        dpp_parser_plc_profile_t *plc_profile = &dpp_parser_plc_profiles[i_prf];

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        PRT_CELL_SET("%s", plc_profile->name);
        for (i_plc = 0; i_plc < plc_profile->nof_plc; i_plc++)
        {
            dpp_parser_plc_info_t *plc_info = &plc_profile->plc_info[i_plc];
            if(i_plc != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", plc_info->name);
            if(plc_info->type == DPP_PLC_TYPE_GROUP)
            {
                PRT_CELL_SET("Gr");
            }
            else
            {
                PRT_CELL_SET("Pr");
            }
            PRT_CELL_SET("0x%02X", plc_info->sw);
            PRT_CELL_SET("0x%02X", plc_info->hw);
            PRT_CELL_SET("0x%02X", plc_info->mask);
       }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_proto_eth_man = {
   "Present ethernet protocols used by parser",
   "Command presents configurable ethernet protocols used by Macro",
   NULL,
   NULL
};

static shr_error_e
parser_proto_eth_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_proto;
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           eth_protocols;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Ethernet Protocols");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("ProtoEth2");
    PRT_COLUMN_ADD("Proto802.3");

    for(i_proto = 0; i_proto < ARAD_PARSER_ETHER_PROTO_NOF_ENTRIES; i_proto++) {
        arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(unit, i_proto, &eth_protocols);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", i_proto);
        PRT_CELL_SET("0x%04X", eth_protocols.eth_type_protocol);
        PRT_CELL_SET("0x%04X", eth_protocols.eth_sap_protocol);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_proto_ip_man = {
   "Present ip protocols used by parser",
   "Command presents configurable ip protocols used by Macro",
   NULL,
   NULL
};

static shr_error_e
parser_proto_ip_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_proto;
    ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA            ip_protocols;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("IP Protocols");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Proto");

    for(i_proto = 0; i_proto < ARAD_PARSER_IP_PROTO_NOF_ENTRIES; i_proto++) {
        arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe(unit, i_proto, &ip_protocols);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", i_proto);
        PRT_CELL_SET("0x%04X", ip_protocols.ip_protocol);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_proto_custom_man = {
   "Present custom protocols used by parser",
   "Command presents configurable custom protocols used by Macro",
   NULL,
   NULL
};

static shr_error_e
parser_proto_custom_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i_proto;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  custom_macro_protocols;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Custom Protocols");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Proto");

    for(i_proto = 0; i_proto < ARAD_PARSER_CUSTOM_PROTO_NOF_ENTRIES; i_proto++) {
        arad_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(unit, i_proto, &custom_macro_protocols);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", i_proto);
        PRT_CELL_SET("0x%04X", custom_macro_protocols.cstm_protocol);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t parser_proto_man = {
   "Present protocols used for programmable parser",
   NULL,
   NULL,
   NULL,
};

sh_sand_cmd_t sh_dpp_parser_proto_cmds[] = {
    {"ethernet", parser_proto_eth_cmd,    NULL,                   NULL,                   &parser_proto_eth_man},
    {"ip",       parser_proto_ip_cmd,     NULL,                   NULL,                   &parser_proto_ip_man},
    {"custom",   parser_proto_custom_cmd, NULL,                   NULL,                   &parser_proto_custom_man},
    {NULL}
};

sh_sand_cmd_t sh_dpp_parser_cmds[] = {
    {"segment",   parser_segment_cmd,   NULL,                     parser_segment_options, &parser_segment_man},
    {"macro",     parser_macro_cmd,     NULL,                     parser_macro_options,   &parser_macro_man},
    {"pfc",       parser_pfc_cmd,       NULL,                     NULL,                   &parser_pfc_man},
    {"plc",       parser_plc_cmd,       NULL,                     NULL,                   &parser_plc_man},
    {"protocol",  NULL,                 sh_dpp_parser_proto_cmds, NULL,                   &parser_proto_man},
    {NULL}
};

/*
 * This routine is for DPP/DFE only - For DNX/DNXF recursion starts from the top
 */
cmd_result_t
cmd_dpp_parser(
    int unit,
    args_t * args)
{
    cmd_result_t result;
    SH_SAND_VERIFY("PaRSer", sh_dpp_parser_cmds, result);

    sh_sand_act(unit, args, sh_dpp_parser_cmds, sh_sand_sys_cmds, NULL);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;
}
