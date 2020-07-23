/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_oam.c
 * Purpose: Manages oam diagnostics functions 
 */


#include <shared/bsl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/diag_oam.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_prt.h>

#ifdef BCM_PETRA_SUPPORT

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/bfd_uc.h>
#include <bcm_int/dpp/utils.h>


/*
 * Dispalay oam endpoints.
*/
cmd_result_t
cmd_ppd_api_diag_oam_endpoint(int unit, args_t *a) {
    char *next_arg;
    int mepID;

    next_arg = ARG_GET(a);
    if (next_arg) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(next_arg, "id=",sal_strlen("id="))) {
            mepID = sal_strtoul(next_arg + 3, NULL, 0);

            if ((_bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(unit, mepID) != 0)) {
                cli_out("Diag failed. No endpoint with id %d found\n", mepID);
            }
        } else { /*Invalid parameter given*/
            return CMD_USAGE;
        }
    } else { /* no parameter given. print all endpoints*/
        if (_bcm_dpp_oam_bfd_diagnostics_endpoints(unit) != 0) {
            cli_out("Diag failed. Was OAM/BFD initialized?\n");
            return CMD_FAIL;
        }
    }
    return 0;

}

/*
 * Dispalay oam dma_dump memory.
*/
cmd_result_t
cmd_ppd_api_diag_oam_dma_dump(int unit) {

    if (SOC_IS_JERICHO(unit))
    {
        cli_out("Diag failed. The Diag is supported only for Jericho and above devices?\n");
        return CMD_FAIL;
    }
    return soc_ppd_oam_diag_print_dma_dump_unsafe(unit);

    return 0;

}

/*
 * Display oam lookup
*/
cmd_result_t
cmd_ppd_api_diag_oam_lookup(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);

    if (next_arg) {
       return CMD_USAGE;
    }

    return soc_ppd_oam_diag_print_lookup(unit);

    return 0;
}


typedef struct _diag_oam_counters_oamp_set_opts_type_str_to_type_s {
    char *type_str;
    int  type;
}diag_oam_counters_oamp_set_opts_type_str_to_type;

typedef struct _diag_oam_counters_oamp_set_opts_filter_str_to_filter_s {
    char *filter_str;
    int  filter;
}diag_oam_counters_oamp_set_opts_filter_str_to_filter;

/* Table of type string to type mapping */
diag_oam_counters_oamp_set_opts_type_str_to_type type_str_to_type[] = 
{
    {"Rx", RX},
    {"Tx", TX},
    {"Punt_response", PUNT_RESPONSE}
};

#define SIZE_OF_TYPE_STR_TO_TYPE_TBL (sizeof(type_str_to_type)/sizeof(diag_oam_counters_oamp_set_opts_type_str_to_type))

/* Table of filter string to filter mapping */
diag_oam_counters_oamp_set_opts_filter_str_to_filter filter_str_to_filter[] = 
{
    {"All", ALL},
    {"MEP", MEP},
    {"RMEP", RMEP},
    {"Opcode", OPCODE},
    {"CCM", CCM},
    {"LMM", LMM},
    {"DMM", DMM},
    {"SLM", SLM},
    {"BFD", BFD},
    {"Punt", PUNT},
    {"Response", RESPONSE},
    {"Response_Mep", RESPONSE_MEP},
    {"Response_Opcode", RESPONSE_OPCODE}
};

#define SIZE_OF_FILTER_STR_TO_FILTER_TBL (sizeof(filter_str_to_filter)/sizeof(diag_oam_counters_oamp_set_opts_filter_str_to_filter))

/* This API looks up the type_str in the mapping table and returns the appropriate type */
int parse_diag_oam_counters_oamp_set_opts_from_type_str_to_type_val(char *type_str, int *type)
{
    int found = 0;
    int i;
    for (i = 0; i < SIZE_OF_TYPE_STR_TO_TYPE_TBL; i++) {
        if (!sal_strcasecmp(type_str, type_str_to_type[i].type_str)) {
            found = 1;
            *type = type_str_to_type[i].type;
            break;
        }
    }
    if (!found) {
        return -1;
    }
    return 0;
}

/* This API looks up the filter_str in the mapping table and returns the appropriate filter */
int parse_diag_oam_counters_oamp_set_opts_from_filter_str_to_filter_val(char *filter_str, int *filter)
{
    int found = 0;
    int i;
    for (i = 0; i < SIZE_OF_FILTER_STR_TO_FILTER_TBL; i++) {
        if (!sal_strcasecmp(filter_str, filter_str_to_filter[i].filter_str)) {
            found = 1;
            *filter = filter_str_to_filter[i].filter;
            break;
        }
    }
    if (!found) {
        return -1;
    }
    return 0;
}

/* This API looks up the type in the mapping table and returns the appropriate type string */
int parse_diag_oam_counters_oamp_set_opts_from_type_to_type_str_val(char *type_str, int type)
{
    int found = 0;
    int i;
    int len = 0;
    for (i = 0; i < SIZE_OF_TYPE_STR_TO_TYPE_TBL; i++) {
        if (type_str_to_type[i].type == type) {
            found = 1;
            len = strlen(type_str_to_type[i].type_str);
            sal_strncpy(type_str, type_str_to_type[i].type_str, len);
            break;
        }
    }
    if (!found) {
        return -1;
    }
    return 0;
}

/* This API looks up the filter in the mapping table and returns the appropriate filter string */
int parse_diag_oam_counters_oamp_set_opts_from_filter_to_filter_str_val(char *filter_str, int filter)
{
    int found = 0;
    int i;
    int len = 0;
    for (i = 0; i < SIZE_OF_FILTER_STR_TO_FILTER_TBL; i++) {
        if (filter_str_to_filter[i].filter == filter) {
            found = 1;
            len = strlen(filter_str_to_filter[i].filter_str);
            sal_strncpy(filter_str, filter_str_to_filter[i].filter_str, len);
            break;
        }
    }
    if (!found) {
        return -1;
    }
    return 0;
}

/* Given the type string and filter string, get the type and filter */
int parse_diag_oam_counters_oamp_set_opts_from_str_to_vals(char *type_str, char *filter_str, int *type, int *filter)
{
    int rv = 0;
    rv = parse_diag_oam_counters_oamp_set_opts_from_type_str_to_type_val(type_str, type);
    if (rv < 0) {
        return rv;
    }
    rv = parse_diag_oam_counters_oamp_set_opts_from_filter_str_to_filter_val(filter_str, filter);
    if (rv < 0) {
        return rv;
    }
    return 0;
}

/* Given the diag count oamp set command, parses the type string and filter string and value from the command */
void parse_diag_oam_counters_oamp_set_opts(int unit, args_t *a, char *type_str, char *filter_str, int *value)
{
    char *next_arg= ARG_GET(a);
    int len = 0;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strncasecmp(next_arg, "type=", sal_strlen("type=")))) {
        len=strlen(next_arg);
        sal_strncpy(type_str, next_arg+5, len-5);
    }

    next_arg= ARG_GET(a);
    if (next_arg && (!sal_strncasecmp(next_arg, "filter=", sal_strlen("filter=")))) {
        len=strlen(next_arg);
        sal_strncpy(filter_str, next_arg+7, len-7);
    }

    next_arg= ARG_GET(a);
    if (next_arg && (!sal_strncasecmp(next_arg, "value=", sal_strlen("value=")))) {
         *value = sal_strtoul(next_arg + 6, NULL, 0);
    } 
}

/* Given the diag count oamp command, parses the command and finds if it is help, show or set command
 * and if it is set, find the type, filter and value.
 */
void parse_diag_oam_counters_oamp_opts(int unit, args_t *a, int *set , int *show , int *help , int *oamp, int *set_type, int *set_filter, int *set_value)
{
    char *next_arg= ARG_GET(a);
    char type_str[20] = {0}, filter_str[20] = {0};
    int type = -1, filter = -1;
    int value = -1;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strcasecmp(next_arg, "set"))) {
        *set = 1;
        parse_diag_oam_counters_oamp_set_opts(unit, a, type_str, filter_str, &value);
        if (parse_diag_oam_counters_oamp_set_opts_from_str_to_vals(type_str, filter_str, &type, &filter) < 0) {
            cli_out("String to value conversion failed\r\n");
            *set = 0;
        } else {
            *set_type = type;
            *set_filter = filter;
            *set_value = value;
        }
    } else if (next_arg && (!sal_strcasecmp(next_arg, "show"))) {
        *show = 1;
    } else if (next_arg && (!sal_strcasecmp(next_arg, "help"))) {
        *help = 1;
    } else {
        *oamp = 1;
    } 
    
}

typedef struct _bcm_dpp_diag_print_oamp_count_show_info_s {
    char type_str[20];
    char filter_str[20];
    uint16  value;
    uint32 counter_val;
}_bcm_dpp_diag_print_oamp_count_show_info_t;


#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/* Prints the diag oamp counter show command in proper format */
int _bcm_dpp_oam_diag_print_oamp_count_formatted(int unit,
                                                 _bcm_dpp_diag_print_oamp_count_show_info_t *info,
                                                 int num_info)
{
    int info_idx;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("OAM COUNT OAMP SHOW");

    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Filter");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Counter");

    for (info_idx = 0; info_idx < num_info; info_idx++) {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", info[info_idx].type_str);
        PRT_CELL_SET("%s", info[info_idx].filter_str);
        if (info[info_idx].value != 0) {
            PRT_CELL_SET("%u", info[info_idx].value);
        } else {
            PRT_CELL_SET("%s", "-");
        }
        PRT_CELL_SET("0x%x", info[info_idx].counter_val);
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* API that handles diag oamp counter show command */
void parse_diag_oam_counters_oamp_show(int unit)
{
    uint8 type;
    uint8 filter;
    uint16 value;
    uint32 counter_val;
    _bcm_dpp_diag_print_oamp_count_show_info_t show_info[3]; /* Max 3 types. RX TX PUNT_RESPONSE */
    int num_info = 0;
    int rv = 0;

    for (type = RX; type <= PUNT_RESPONSE; type++) { 
        value = 0;
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_qax_diag_oamp_counter_get, (unit, type, &filter, &value, &counter_val));
        if (rv != 0) {
            cli_out("Get HW call failed for type %d\r\n", type);
            return;
        }
        sal_memset(&(show_info[num_info]), 0, sizeof(_bcm_dpp_diag_print_oamp_count_show_info_t));
        rv = parse_diag_oam_counters_oamp_set_opts_from_type_to_type_str_val(show_info[num_info].type_str, type);
        if (rv < 0) {
            cli_out("Invalid type!! %d\r\n", type);
            return;
        }
        rv = parse_diag_oam_counters_oamp_set_opts_from_filter_to_filter_str_val(show_info[num_info].filter_str, filter);
        if (rv < 0) {
            cli_out("Invalid filter!! %d\r\n", filter);
            return;
        }
        show_info[num_info].value = value;
        show_info[num_info].counter_val = counter_val;
        num_info++;
    }
    rv = _bcm_dpp_oam_diag_print_oamp_count_formatted(unit, show_info, num_info);
    if (rv != 0) {
        cli_out("PRT failed\r\n");
    }
    return; 
}

/* Api that handles diag oam count oamp help command */
int parse_diag_oam_counters_oamp_help(int unit)
{
    char type_str[20];
    char filter_help_str_rx[4][70] = {"All - Count all packets",
                                      "MEP - Filter by MEP-ID",
                                      "RMEP - Filter by RMEP-ID",
                                      "Opcode - Filter by opcode"};
    char filter_help_str_tx[7][70] = {"All - Count all packets",
                                      "MEP - Filter by MEP-ID",
                                      "CCM - Count CCM packets",
                                      "LMM - Count Y.1731 LMM packets",
                                      "DMM - Count Y.1731 DMM packets",
                                      "SLM - Count Y.1731 SLM packets",
                                      "BFD - Count BFD packets"
                                    };
    char filter_help_str_pr[5][70] = {"All - Count all packets", 
                                      "Punt              - Count all Punt packets",
                                      "Response          - Count all generated response packet",
                                      "Response_Mep      - Count gen. response packet filtered by MEP-ID",
                                      "Response_Opcode   - Count gen. response packet filtered by opcode"
                                     };
    int type = 0;
    int rv = 0, i;
    int num_filters[3] = {4, 7, 5};
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("OAM COUNT OAMP HELP");

    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Filter");

    for (type = RX; type <= PUNT_RESPONSE; type++) { 
        for (i = 0; i < num_filters[type-1]; i++) {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            sal_memset(type_str, 0, 20);
            rv = parse_diag_oam_counters_oamp_set_opts_from_type_to_type_str_val(type_str, type);
            if (rv < 0) {
                continue;
            }
            if (i == 0) { /* First cell, print the type name */
                PRT_CELL_SET("%s", type_str);
            } else {
                PRT_CELL_SKIP(1);
            }
            if (type == RX) {
                PRT_CELL_SET("%s", filter_help_str_rx[i]);
            }else if (type == TX) {
                PRT_CELL_SET("%s", filter_help_str_tx[i]);
            } else if (type == PUNT_RESPONSE) {
                PRT_CELL_SET("%s", filter_help_str_pr[i]);
            }
        }
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Display oam  counters
*/
cmd_result_t
cmd_ppd_api_diag_oam_counters(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);
    int mepID;int set = 0, show = 0, help = 0, oamp = 0, set_type = -1, set_filter = -1, set_value = -1;
    int rv = 0;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strncasecmp(next_arg, "id=", sal_strlen("id=")))) {
         mepID = sal_strtoul(next_arg + 3, NULL, 0);
         if (_bcm_dpp_oam_bfd_diagnostics_LM_counters(unit,mepID) !=0 ) {
             cli_out("Diag failed. No OAM endpoint with id %d found\n", mepID);
         }
    } else if (next_arg && (!sal_strcasecmp(next_arg, "oamp"))){
        parse_diag_oam_counters_oamp_opts(unit, a, &set, &show, &help, &oamp, &set_type, &set_filter, &set_value);
        if (set && SOC_IS_QAX(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_qax_diag_oamp_counter_set, (unit, set_type, set_filter, set_value));
            if (rv != 0) {
                cli_out("Set HW call failed\r\n");
            } 
        } else if (show && SOC_IS_QAX(unit)) {
            parse_diag_oam_counters_oamp_show(unit);
        } else if (help && SOC_IS_QAX(unit)) {
            rv = parse_diag_oam_counters_oamp_help(unit);
            if (rv != 0) {
                cli_out("Help command failed\r\n");
            }
        } else if (oamp) {
            return soc_ppd_oam_diag_print_oamp_counter(unit);
        } else {
            cli_out("Invalid command issued\r\n");
        }
    } else {
        return CMD_USAGE;
    }

    return 0;
}

/*
 * Display OAM counters by LIF, YDV, ING.
 * Useful for hierarchical counting.
 */

cmd_result_t
cmd_ppd_api_diag_oam_hier_counters(int unit, args_t *a) {
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY key_params = {0};
    parse_table_t pt;

    int oam_lif = -1;
    int ingress = 1;

    if (!SOC_IS_JERICHO_PLUS(unit)) {        
        cli_out("Hierarchical counting is supported only from Jericho plus\n");
        return CMD_OK;
    }


    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "lif",    PQ_DFL | PQ_INT, &oam_lif,       &oam_lif,       NULL);
    parse_table_add(&pt, "ing",    PQ_DFL | PQ_INT, &ingress,       &ingress,       NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    key_params.oam_lif = oam_lif;
    key_params.ingress = ingress;

    if (key_params.oam_lif > OAM_LIF_MAX_VALUE(unit)) {
        cli_out("lif is not in range\n");
        return CMD_OK;
    }

    if (key_params.ingress > 1) {
        cli_out("ing is out of range (use 0 for ingress and 1 for egress)\n");
        return CMD_OK;
    }


    cli_out("\nFull command issued: diag oam hic lif=0x%0x ing=%d\n",\
            key_params.oam_lif, key_params.ingress);

    if (_bcm_dpp_oam_diag_print_hier_counters(unit, &key_params) != 0) {
        cli_out("Diag failed.\n");
    }
    return 0;
}

cmd_result_t
cmd_ppd_api_diag_oam_rx_packet(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);
    int core;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strncasecmp(next_arg, "core=", sal_strlen("core=")))) {
         core = sal_strtoul(next_arg + 5, NULL, 0);
    } else {
        return CMD_USAGE;
    }

	if ((core < 0 )  
        || 
      (core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
         cli_out("error ilegal core ID for device\n");
         return CMD_FAIL;
    } 


    return soc_ppd_oam_diag_print_rx(unit, core);
}

cmd_result_t
cmd_ppd_api_diag_oam_em(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);
    int lif;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strncasecmp(next_arg, "lif=", strlen("lif=")))) {
        lif = sal_strtoul(next_arg + 4, NULL, 0);
        if (lif < 0 || lif > OAM_LIF_MAX_VALUE(unit)) {
           cli_out("lif is out of range\n");
           return CMD_OK;
        }
        if (soc_ppd_oam_diag_print_em(unit,lif) !=0 ) {
            cli_out("Diag failed.\n");
        }
    } else {
        return CMD_USAGE;
    }

    return 0;
}


cmd_result_t
cmd_ppd_api_diag_oam_ak(int unit, args_t *a) {
    SOC_PPC_OAM_ACTION_KEY_PARAMS key_params = {0};
    parse_table_t pt;

    key_params.lif = -1;
    key_params.level = 0;
    key_params.opcode = 1;
    key_params.ing = 1;
    key_params.inj = 0;
    key_params.mymac = 0;
    key_params.bfd = 0;
    key_params.your_disc = 0;

    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "lif",   PQ_DFL | PQ_INT, &key_params.lif,       &key_params.lif,       NULL);
    parse_table_add(&pt, "mdl",   PQ_DFL | PQ_INT, &key_params.level,     &key_params.level,     NULL);
    parse_table_add(&pt, "op",    PQ_DFL | PQ_INT, &key_params.opcode,    &key_params.opcode,    NULL);
    parse_table_add(&pt, "ing",   PQ_DFL | PQ_INT, &key_params.ing,       &key_params.ing,       NULL);
    parse_table_add(&pt, "inj",   PQ_DFL | PQ_INT, &key_params.inj,       &key_params.inj,       NULL);
    parse_table_add(&pt, "mymac", PQ_DFL | PQ_INT, &key_params.mymac,     &key_params.mymac,     NULL);
    parse_table_add(&pt, "bfd",   PQ_DFL | PQ_INT, &key_params.bfd,       &key_params.bfd,       NULL);
    parse_table_add(&pt, "ydv",   PQ_DFL | PQ_INT, &key_params.your_disc, &key_params.your_disc, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (key_params.lif < 0 || key_params.lif > OAM_LIF_MAX_VALUE(unit)) {
        cli_out("lif is not in range\n");
        return CMD_OK;
    }

    if (key_params.your_disc != 0 && !SOC_IS_JERICHO(unit)) {
        cli_out("ydv is only available in Jericho devices or above\n");
        return CMD_OK;
    }

    if (key_params.bfd) {
        if (key_params.level) {
            cli_out("mdl should not be set if packet is a BFD packet\n");
            return CMD_OK;
        }
        if (key_params.opcode) {
            cli_out("op should not be set if packet is a BFD packet\n");
            return CMD_OK;
        }
        if (!key_params.ing) {
            cli_out("ing should be 1 if packet is a BFD packet\n");
            return CMD_OK;
        }
    } else {
        if (key_params.your_disc) {
            cli_out("ydv should not be set if packet is not a BFD packet\n");
            return CMD_OK;
        }
    }

    if (key_params.level < 0 || key_params.level > 7) {
        cli_out("mdl is out of range (valid range is 0-7)\n");
        return CMD_OK;
    }

    if (key_params.opcode < 0 || key_params.opcode > 255) {
        cli_out("op is out of range (valid range is 1-255, 0 for bfd)\n");
        return CMD_OK;
    }

    if (key_params.ing < 0 || key_params.ing > 1) {
        cli_out("ing is out of range (use 0 for ingress and 1 for egress)\n");
        return CMD_OK;
    }

    if (key_params.inj < 0 || key_params.inj > 1) {
        cli_out("inj is out of range (use 1 for injected packets, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.mymac < 0 || key_params.mymac > 1) {
        cli_out("mymac is out of range (use 1 if DA on packet equals MEP's address, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.bfd < 0 || key_params.bfd > 1) {
        cli_out("bfd is out of range (use 1 for BFD packet, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.your_disc < 0 || key_params.your_disc > 1) {
        cli_out("ydv is out of range (use 1 if Your-Discriminator on packet equals MEP's My-Discriminator, otherwise use 0)\n");
        return CMD_OK;
    }

    cli_out("\nFull command issued: diag oam cl lif=0x%0x mdl=%d op=0x%0x ing=%d inj=%d mymac=%d bfd=%d ydv=%d\n",\
            key_params.lif, key_params.level, key_params.opcode, key_params.ing, key_params.inj, key_params.mymac,\
            key_params.bfd, key_params.your_disc);
    if (soc_ppd_oam_diag_print_ak(unit, &key_params) != 0) {
        cli_out("Diag failed.\n");
    }

    return 0;
}


cmd_result_t
cmd_ppd_api_diag_oam_ks(int unit, args_t *a) {
    SOC_PPC_OAM_KEY_SELECT_PARAMS key_params = {0};
    parse_table_t pt;

    key_params.ing = 1;

    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "ing", PQ_DFL | PQ_INT, &key_params.ing, &key_params.ing, NULL);

    parse_table_add(&pt, "olo", PQ_DFL | PQ_INT, &key_params.olo, &key_params.olo, NULL);
    parse_table_add(&pt, "oli", PQ_DFL | PQ_INT, &key_params.oli, &key_params.oli, NULL);
    /*Egress only*/
    parse_table_add(&pt, "pio", PQ_DFL | PQ_INT, &key_params.pio, &key_params.pio, NULL);
    parse_table_add(&pt, "cp",  PQ_DFL | PQ_INT, &key_params.cp,  &key_params.cp,  NULL);
    parse_table_add(&pt, "inj", PQ_DFL | PQ_INT, &key_params.inj, &key_params.inj, NULL);
    /*Ingress only*/
    parse_table_add(&pt, "leo", PQ_DFL | PQ_INT, &key_params.leo, &key_params.leo, NULL);
    parse_table_add(&pt, "lei", PQ_DFL | PQ_INT, &key_params.lei, &key_params.lei, NULL);
    parse_table_add(&pt, "ydv", PQ_DFL | PQ_INT, &key_params.ydv, &key_params.ydv, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (key_params.ing) {
        if (key_params.inj || key_params.pio || key_params.cp) {
            cli_out("Egress argument are set for Ingress mode.\n");
            return CMD_OK;
        }
    } else {
        if (key_params.ydv || key_params.leo || key_params.lei) {
            cli_out("Ingress argument are set for Egress mode.\n");
            return CMD_OK;
        }
    }

    if (key_params.ing < 0 || key_params.ing > 1 ||
        key_params.olo < 0 || key_params.olo > 1 ||
        key_params.oli < 0 || key_params.oli > 1 ||
        key_params.inj < 0 || key_params.inj > 1 ||
        key_params.pio < 0 || key_params.pio > 1 ||
        key_params.cp < 0 || key_params.cp > 1 ||
        key_params.ydv < 0 || key_params.ydv > 1 ||
        key_params.leo < 0 || key_params.leo > 1 ||
        key_params.lei < 0 || key_params.lei > 1) {
        cli_out("Input parameters should be 0 or 1.\n");
        return CMD_OK;
    }

    if (key_params.ing) {
        cli_out("\nFull command issued: diag oam ks ing=1 olo=%d oli=%d leo=%d lei=%d ydv=%d\n",
                key_params.olo, key_params.oli, key_params.leo, key_params.lei, key_params.ydv);
    } else {
        cli_out("\nFull command issued: diag oam ks ing=0 olo=%d oli=%d pio=%d cp=%d inj=%d\n",
                key_params.olo, key_params.oli, key_params.pio, key_params.cp, key_params.inj);
    }

    if (soc_ppd_oam_diag_print_ks(unit, &key_params) != 0) {
        cli_out("Diag failed.\n");
    }

    return 0;
}

/* Assumption is n >= m  and bit position is expected to be mentioned in 0 to n-1 range for a n-sized bitmap */
#define GET_M_TO_NTH_BITS_FROM_VAL(val, n, m) ((val & (((1 << (n-m+1)) -1) << m)) >> m)

cmd_result_t
cmd_ppd_api_diag_oam_parse_oamak(int unit, args_t *a) {
    parse_table_t pt;
    uint32 key = 0;
    uint8 mp_type = 0, mep_type = 0, mip_type = 0;


    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "key",   PQ_DFL | PQ_INT, &key, &key,       NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (key & ~(0x3FFF)) {
        cli_out("Invalid key\n");
        return CMD_OK;
    }

    /* Only 14 bits are valid */
    key = key & (0x3FFF);

    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        mp_type = (GET_M_TO_NTH_BITS_FROM_VAL(key, 11,11) << 2) | GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4);
        if (SOC_IS_JERICHO_PLUS(unit)) {
            cli_out("MP-Type: %d (%s)\n",
                        mp_type,
                        qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str_get(mp_type));
        } else {
            cli_out("MP-Type: %d (%s)\n",
                        mp_type,
                        jericho_pp_oam_diag_mp_type_to_mp_type_str_get(mp_type));
        }
        /* Format : Inject(1b)|My-CFM-MAC(1b)|MSB of MP-Type(1b)|Opcode(4b)|Ingress(1b)|2 LSB bits of MP-Type(2b)|MP_PROFILE(4b) */
        cli_out("\nInject[1b]=%u | My_CFM_Mac[1b]=%u | MSB of MP-Type[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | LSB bits of MP-Type[2b]=%u |  MP_Profile[4b]=%u\n",
                GET_M_TO_NTH_BITS_FROM_VAL(key, 13,13), 
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 12,12),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 11,11),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 10,7),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 6,6),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 3,0));
    } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
        mp_type = GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4);
        cli_out("MP-Type: %d (%s)\n",
                mp_type,
                arad_plus_pp_oam_diag_mp_type_to_mp_type_str_get(mp_type));
        /* Format : Inject(1b)|My-CFM-MAC(1b)|Is BFD(1b)|Opcode(4b)|Ingress(1b)| MP-Type(2b)|MP_PROFILE(4b) */
        cli_out("\nInject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | MP-Type[2b]=%u |  MP_Profile[4b]=%u\n",
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 13,13), 
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 12,12),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 11,11),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 10,7),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 6,6),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 3,0)); 
    } else {
        mep_type = GET_M_TO_NTH_BITS_FROM_VAL(key, 3,2);
        mip_type = GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4);
        cli_out("MEP-Type: %d (%s)\n",
                mep_type,
                arad_pp_oam_diag_mp_type_to_mp_type_str_get(mep_type));
        cli_out("MIP-Type: %d (%s)\n",
                mip_type,
                arad_pp_oam_diag_mp_type_to_mp_type_str_get(mip_type));
        /* Format : Inject(1b)|My-CFM-MAC(1b)|Is BFD(1b)|Opcode(4b)|Ingress(1b)| MIP-Type(2b)| MEP-Type(2b)|MP_PROFILE(2b) */
        cli_out("\nInject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | MIP-Type[2b]=%u | MEP-Type[2b]=%u | MP_Profile[2b]=%u\n",
                GET_M_TO_NTH_BITS_FROM_VAL(key, 13,13), 
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 12,12),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 11,11),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 10,7),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 6,6),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 5,4),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 3,2),
                 GET_M_TO_NTH_BITS_FROM_VAL(key, 1,0));
    }
    return 0;
}

cmd_result_t
cmd_ppd_api_diag_oam_parse_oambk(int unit, args_t *a) {
    parse_table_t pt;
    uint32 key = 0;


    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "key",   PQ_DFL | PQ_INT, &key, &key,       NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (key & ~(0xFFF)) {
        cli_out("Invalid key\n");
        return CMD_OK;
    }

    /* Only 12 bits are valid */
    key = key & (0xFFF);

    /* Format : Inject(1b)|My-CFM-MAC(1b)|Is BFD(1b)|Opcode(4b)|Ingress(1b)| MP_PROFILE(4b) */
    cli_out("\nInject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u | Opcode[4b]=%u | Ingr[1b]=%u | Acc_MP_Profile[4b]=%u\n",
            GET_M_TO_NTH_BITS_FROM_VAL(key, 11,11),
            GET_M_TO_NTH_BITS_FROM_VAL(key, 10,10),
            GET_M_TO_NTH_BITS_FROM_VAL(key, 9,9),
            GET_M_TO_NTH_BITS_FROM_VAL(key, 8,5),
            GET_M_TO_NTH_BITS_FROM_VAL(key, 4,4),
            GET_M_TO_NTH_BITS_FROM_VAL(key, 3,0));
    return 0;
}

cmd_result_t
cmd_ppd_api_diag_oam_prge(int unit, args_t *a) {

    parse_table_t    pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_oamp_pe_print_last_program_data(unit);
    } else {
        res = arad_pp_oamp_pe_print_all_programs_data(unit);
    }    
    /* check if previous function return an error value */
    if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
        return CMD_FAIL; 
    }
	
	return 0;    
}

cmd_result_t
cmd_ppd_api_diag_oam_debug(int unit, args_t *a) {
    parse_table_t    pt;
    uint32         default_mode = 0;
    uint32         mode = 0;
    uint32         is_cfg = 0;

    if (ARG_CNT(a) != 0) {
        is_cfg = 1;
    }

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_DFL|PQ_INT, &default_mode,  &mode, NULL);  
    
    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    return soc_ppd_oam_diag_print_debug(unit, is_cfg, mode);
}

cmd_result_t
cmd_ppd_api_diag_oam_oam_id(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);
    int core;

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (next_arg && (!sal_strncasecmp(next_arg, "core=", sal_strlen("core=")))) {
         core = sal_strtoul(next_arg + 5, NULL, 0);
    } else {
        return CMD_USAGE;
    }
	if ((core < 0 )  
        || 
      (core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
         cli_out("error ilegal core ID for device\n");
         return CMD_FAIL;
    } 

    return soc_ppd_oam_diag_print_oam_id(unit, core);
}

cmd_result_t
cmd_ppd_api_diag_oam_parse_tcam(int unit, args_t *a) {
    int core=0;

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) { 
        char *next_arg= ARG_GET(a);

        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (next_arg && (!sal_strncasecmp(next_arg, "core=", sal_strlen("core=")))) {
            core = sal_strtoul(next_arg + 5, NULL, 0);
        } else {
            return CMD_USAGE;
        }

        if ((core < 0 ) || (core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
            cli_out("error ilegal core ID for device\n");
            return CMD_FAIL;
        }
    }
    
    return soc_ppd_oam_diag_print_tcam_entries(unit, core);
}

cmd_result_t
cmd_ppd_api_diag_oam_dump_hw_tcam(int unit) {

    return soc_ppd_oam_diag_print_hw_tcam_entries(unit);
}
/*
 * Print diag oam usage
*/
void print_oam_usage(int unit) {
    char cmd_dpp_diag_oam_usage[] =
       "Usage (DIAG oam):"
       "\n\tDIAGnotsics OAM commands\n\t"
       "Usages:\n\t"
       "DIAG oam <OPTION> <parameters> ..."
#ifdef __PEDANTIC__
       "\nFull documentation cannot be displayed with -pendantic compiler"
#else
       "OPTION can be:"
       "\n\nEndPoints- \tDisplay information on all endpoints and their remote endpoints"
       "\n\tOptional parameters:"
       "\n\t\tID=<endpoint id> - Display an OAM endpoint according to the endpoint ID."
       "\n\nLookUps- \tDisplay lookup information from OAM exact matches."
       "\n\nCOUNTers- \tDisplay OAM LM counters for a given endpoint ID."
       "\n\tParameters:"
       "\n\t\tID=<endpoint id> - endpoint for which counters are to be displayed"
	   "\n\t\tOAMP - Display OAMP counters"
	   "\n\t\t\t\thelp - print usage of OAMP counter filters (QAX only)"
	   "\n\t\t\t\tshow - print OAMP counter filters, filter values and counters (QAX only)"
	   "\n\t\t\t\tset - set OAMP counter filters (QAX only)"
	   "\n\t\t\t\t\t\ttype=[Rx|Tx|Punt_response]  - type of OAMP counter (QAX only)"
	   "\n\t\t\t\t\t\tfilter=[All|MEP|RMEP|Opcode|<Tx pkt_type>] - type of OAMP counter filter (QAX only)"
	   "\n\t\t\t\t\t\tvalue=<value> - value for OAMP counter filter (QAX only)"
       "\n\nHIerarchical_Counters- \tDisplay OAM LM counters for a given combination of LIF, Direction."
       "\n\tParameters:"
       "\n\t\tLIF=<LIF> - Key for O-EM-1 entry"
	   "\n\t\tING=<0/1> - Is classification in Ingress/Egress? (default is 1)"
       "\n\nRX_packet- \tDisplay information about the last RX OAM packet."
       "\n\nExact_Match-\tDisplay O-EM-1 entries by LIF."
       "\n\tParameters:"
       "\n\t\tLIF=<LIF> Key for O-EM-1 entry"
       "\n\nKey_Select-\tSimulates OAM key selection for O-EM-1/2 based on signals inputs."
           "\n\t\tIngress side and egress side have different inputs."
       "\n\tParameters:"
       "\n\t\tING   = <yes/no> Is classification in Ingress?              (default is 1)"
       "\n\t\tOLO   = <yes/no> Is OAM-LIF-Outer-Valid?                    (default is 0)"
       "\n\t\tOLI   = <yes/no> Is OAM-LIF-Inner-Valid?                    (default is 0)"
       "\n\t\tPIO   = <yes/no> Packet-Is-OAM?                (Egress only, default is 0)"
       "\n\t\tCP    = <yes/no> Counter-Ptr-Valid?            (Egress only, default is 0)"
       "\n\t\tINJ   = <yes/no> Is inject bit is on?          (Egress only, default is 0)"
       "\n\t\tLEO   = <yes/no> LIF-Equal-To-OAM-LIF-Outer?  (Ingress only, default is 0)"
       "\n\t\tLEI   = <yes/no> LIF-Equal-To-OAM-LIF-Inner?  (Ingress only, default is 0)"
       "\n\t\tYDV   = <yes/no> Your-Disc-Valid?             (Ingress only, default is 0)"
       "\n\nCLassification-\tSimulates OAM classification based on O-EM-1/2 inputs and OAM-1/2 inputs."
           "\n\t\tRetrieves OAM-1/2 action-key & entry."
       "\n\tParameters:"
       "\n\t\tLIF   = <LIF> Key for O-EM-1 entry (mandatory value, must be set, use global lif in Jericho)"
       "\n\t\tMDL   = <Level> Level of the packet                     (default is 0)"
       "\n\t\tOP    = <op-code> OAM Op-Code                           (default is 1 = CCM)"
       "\n\t\tING   = <yes/no> Is classification in Ingress?          (default is 1)"
       "\n\t\tINJ   = <yes/no> Is inject bit is on?                   (default is 0)"
       "\n\t\tMYMAC = <yes/no> Is packet DA equals MEP's MAC address? (default is 0)"
       "\n\t\tBFD   = <yes/no> Is packet is a BFD packet?             (default is 0)"
       "%s" /* For adding line dynamically (for example based on device type) */
       "\n\nPRoGram_Editor-\tDisplay program editor."
       "\n\tParameters:"
       "\n\t\tlast=<0 or 1> Is last invoked PE program."
       "\n\nDeBuG-\tDebug for ARAD mode OAM ID."
       "\n\tParameters:"
       "\n\t\tmode=<0 or 1> Configure ARAD mode OAM ID."
       "\nOAM_ID -\tDisplay for OAM ID."
       "\n\nParse_OamA_Key -\tParse a given OAM-A Key."
       "\n\tParameters:"
       "\n\t\tkey=<key_value> Key value that needs to be parsed."
       "\n\nParse_OamB_Key -\tParse a given OAM-B Key."
       "\n\tParameters:"
       "\n\t\tkey=<key_value> Key value that needs to be parsed."
       "\n\nparse_tcam -\tParse TCAM information."
       "\n\ndump_hw_tcam -\tDump OAM TCAM information."
       "\n\nbfd_ipv6_trace = \tDump uC debug trace."
       "\n\nDMA_dump- \tDisplay information from DMA buffer"
#endif
           "\n";

    cli_out(cmd_dpp_diag_oam_usage,
            SOC_IS_JERICHO(unit) ? "\n\t\tYDV   = <yes/no> Is Your-Disc-Valid set?                (default is 0)" : ""
            );
}

/*
 * "Driver" for the diag oam functionalities.
*/
cmd_result_t
cmd_dpp_diag_oam(int unit, args_t *a) {
    char      *function;

    function = ARG_GET(a);
    if (!function) {
        return CMD_USAGE;
    } else if (DIAG_FUNC_STR_MATCH(function, "endpoints", "ep")) {
        return cmd_ppd_api_diag_oam_endpoint(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "lookups", "lu")) {
        return cmd_ppd_api_diag_oam_lookup(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "counters", "count")) {
        return cmd_ppd_api_diag_oam_counters(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "hierarchical_counters", "hic")) {
        return cmd_ppd_api_diag_oam_hier_counters(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "rx_packet", "rx")) {
        return cmd_ppd_api_diag_oam_rx_packet(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "exact_match", "em")) {
        return cmd_ppd_api_diag_oam_em(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "action_key", "ak")) {
        cli_out("Please use 'diag oam cl' instead\n"); return 0;
    } else if (DIAG_FUNC_STR_MATCH(function, "classification", "cl")) {
        return cmd_ppd_api_diag_oam_ak(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "key_select", "ks")) {
        return cmd_ppd_api_diag_oam_ks(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "program_editor", "prge")) {
        return cmd_ppd_api_diag_oam_prge(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "debug", "dbg")) {
        return cmd_ppd_api_diag_oam_debug(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "oam_id", "oamid")) {
        return cmd_ppd_api_diag_oam_oam_id(unit, a);;
    } else if (DIAG_FUNC_STR_MATCH(function, "parse_oama_key", "poak")) {
        return cmd_ppd_api_diag_oam_parse_oamak(unit, a);;
    } else if (DIAG_FUNC_STR_MATCH(function, "parse_oamb_key", "pobk")) {
        return cmd_ppd_api_diag_oam_parse_oambk(unit, a);;
    } else if (DIAG_FUNC_STR_MATCH(function, "parse_tcam", "ptm")) {
        return cmd_ppd_api_diag_oam_parse_tcam(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "dump_hw_tcam", "dhtm")) {
        return cmd_ppd_api_diag_oam_dump_hw_tcam(unit);
    } else if (DIAG_FUNC_STR_MATCH(function, "dma_dump", "dma")) {
        return cmd_ppd_api_diag_oam_dma_dump(unit);
    } else if (DIAG_FUNC_STR_MATCH(function, "bfd_ipv6_trace", "bfd_ipv6")) {
        _bcm_petra_bfd_uc_dump_trace(unit);
        return CMD_OK;
    } else {
        return CMD_USAGE;
    }

}

#endif /* BCM_PETRA_SUPPORT */
