/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     L2 CLI commands
 */



#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/debug.h>

#include <bcm_int/dpp/l2.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <bcm_int/dpp/utils.h>

STATIC cmd_result_t _cmd_dpp_l2_add(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_clear(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_del(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_init(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_show(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_count(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l2_info(int unit, args_t *args);

#define CMD_L2_USAGE_ADD \
    "    l2 add [MACaddress=<mac>] [Vlanid=<id>]\n" \
    "           [Module=<n>] [PortBitMap=<pbmp>]\n" \
    "           [Trunk=true|false] [TrunkGroupId=<id>]\n" \
    "           [DiscardSource=true|false] [DiscardDest]=true|false]\n" \
    "           [STatic=true|false]\n" \
    "        - Add incrementing L2 addresses associated with port(s)\n"

#define CMD_L2_USAGE_CLEAR \
    "    l2 clear [MACaddress=<mac>] [Vlanid=<id>]\n" \
    "             [Module=<n>] [Port=<port>]\n" \
    "             [TrunkGroupID=<id>] [Static=true|false]\n" \
    "             [all]\n" \
    "        - Remove all L2 entries on the given module/port, MAC,\n" \
    "          VLAN, or trunk group ID or ALL the entries\n"

#define CMD_L2_USAGE_DEL \
    "    l2 del [MACaddress=<mac>] [Vlanid=<id>] [Count=<value>]\n" \
    "        - Delete L2 address(s)\n"

#define CMD_L2_USAGE_INIT \
    "    l2 init\n" \
    "        - Initialize L2\n"

#define CMD_L2_USAGE_SHOW \
    "    l2 show [MACaddress=<mac> Vlanid=<id>] [Count=<value>] [debug=<value>] [sw=<value>]\n" \
    "        - Show L2 addresses starting with given VLAN MAC key\n"

#define CMD_L2_USAGE_REDIRECT \
    "    l2 redirect [MACaddress=<mac>] [Vlanid=<id>]\n" \
    "           [qidunion=<n>]\n" 

#define CMD_L2_USAGE_COUNT \
    "    l2 count vsiNumber=<vsi>\n" \
    "        - to get counter per the given vsi: \n" \
    "        -     give a vsi in between 0-32767 \n" \
    "        -     example: l2 count vsi=0x76 \n" \
	"        - to get total count of entries in mact: \n" \
    "        -     give vsi=-1 \n" \
    "        -     example: l2 count vsi=-1 \n"

#define CMD_L2_USAGE_INFO \
    "    l2 info\n" \
    "        - Display L2 general information\n"

char cmd_dpp_l2_usage[] =
    "\n"
    CMD_L2_USAGE_ADD   "\n"
    CMD_L2_USAGE_CLEAR "\n"
    CMD_L2_USAGE_DEL   "\n"
    CMD_L2_USAGE_INIT  "\n"
    CMD_L2_USAGE_REDIRECT "\n"
    CMD_L2_USAGE_SHOW  "\n"
    CMD_L2_USAGE_COUNT  "\n"
    CMD_L2_USAGE_INFO  "\n"
    ;

static cmd_t _cmd_dpp_l2_list[] = {
    {"ADD",    _cmd_dpp_l2_add,   "\n" CMD_L2_USAGE_ADD,   NULL},
    {"+",      _cmd_dpp_l2_add,   "\n" CMD_L2_USAGE_ADD,   NULL},
    {"CLEAR",  _cmd_dpp_l2_clear, "\n" CMD_L2_USAGE_CLEAR, NULL},
    {"DELete", _cmd_dpp_l2_del,   "\n" CMD_L2_USAGE_DEL,   NULL},
    {"-",      _cmd_dpp_l2_del,   "\n" CMD_L2_USAGE_DEL,   NULL},
    {"INIT",   _cmd_dpp_l2_init,  "\n" CMD_L2_USAGE_INIT,  NULL},
    {"SHOW",   _cmd_dpp_l2_show,  "\n" CMD_L2_USAGE_SHOW,  NULL},
    {"COUNT",  _cmd_dpp_l2_count,   "\n" CMD_L2_USAGE_COUNT,   NULL},
    {"INFO",  _cmd_dpp_l2_info,   "\n" CMD_L2_USAGE_INFO,   NULL}
};



/*
 * Local global variables to remember last values in arguments.
 *
 * Notes:
 * Initialize MAC address field for the user to the first real
 * address which does not conflict.
 */
static sal_mac_addr_t   _l2_macaddr = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};

static int _l2_vlan = VLAN_ID_DEFAULT;

static int _l2_modid = 0, _l2_port, _l2_trunk = 0, _l2_tgid = 0, 
           _l2_dsrc = 0, _l2_ddest = 0, _l2_static = 0;

static pbmp_t  _l2_pbmp;

static const int errorVal = -1;
static const int successVal = 0;

uint32
_arad_pp_frwrd_lem_traverse_check_flush_is_done(int unit,
    SOC_SAND_OUT uint32             *nof_matched_entries,
    SOC_SAND_OUT uint32             *waiting_time_in_ms,
    SOC_SAND_OUT int                *is_wait_success);

/*
* per_vsi_get_count: 
* per vsi get count of entries, reads directly from IHP_MACT_FID_COUNTER_DB 
* vsi=-1 to get the sum of all counts for all vsi's
 */
int
get_mact_num_entries(int unit, uint32 *ret_count)
{
	int res;
	ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA tbl_data;

	res = arad_pp_ihp_mact_entry_count_db_tbl_get_unsafe(unit, &tbl_data); 
	if (res) {
		sal_printf("\nERROR: in arad_pp_ihp_mact_entry_count_db_tbl_get_unsafe\n");
		return errorVal;
	}
	*ret_count = tbl_data.entry_count;
    return successVal;
}

int
per_vsi_get_count(int unit, bcm_vlan_t vsi, uint32 *ret_count)
{
	int res;
	ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA tbl_data;

	if (vsi > 32767) {
			sal_printf("\nERROR: in per_vsi_get_count vsi not in intended range!\n");
			return errorVal;
	}

	res = arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe(unit, vsi, &tbl_data); 
	if (res) {
		sal_printf("\nERROR: in arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe\n");
		return errorVal;
	}
	*ret_count = tbl_data.entry_count;
    return successVal;
}

STATIC cmd_result_t
_cmd_dpp_l2_count(int unit, args_t *args)
{
    cmd_result_t   retCode;
    parse_table_t  pt;
	int vsi = -1;
	uint32 vsi_count = -1;
	int vsi_sum_count = -1;
	char* errMsg1 = "ERROR: in get_mact_num_entries \n";
	char* errMsg2 = "ERROR: in per_vsi_get_count \n";
	char* errMsg;

    /* Parse command option arguments */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Vsi", PQ_DFL|PQ_HEX, &vsi_sum_count, &vsi, NULL);

	if (!parseEndOk(args, &pt, &retCode)) {
        return CMD_USAGE;
    }

	if (vsi < -1 || vsi > 32767) {
		sal_printf("\n vsi not in range! \n");
        return CMD_USAGE;
	}

	if (vsi == -1) {
		retCode = get_mact_num_entries(unit, &vsi_count);
		errMsg = errMsg1;
	}else{
		retCode = per_vsi_get_count(unit, vsi, &vsi_count);
		errMsg = errMsg2;
	}
 
	if (CMD_OK != retCode) {
        cli_out(errMsg);
		return retCode;
	}

	if (vsi == -1) {
		sal_printf("\n==>the total number of entries in mact is %#x\n", vsi_count);
	}else{
		sal_printf("\n==>counter is %#x for vsi %#x\n", vsi_count,vsi);
	}

    return CMD_OK;
}

STATIC cmd_result_t
_cmd_dpp_l2_info(int unit, args_t *args)
{
    int            rv, learn_mode, age_seconds;
    uint32         opport = 0;
	char*          errMsgOpport = "ERROR: in mbcm_pp_frwrd_mact_opport_mode_get \n";
    char*          errMsgLearnMode = "ERROR: in bcm_switch_control_get \n";
    char*          errMsgAge = "ERROR: in bcm_l2_age_timer_get \n";
    char           learn_mode_string[32];

    if (ARG_CNT(args) > 0) {
        cli_out("ERROR: No arguments needed\n");
        return CMD_USAGE;
    }

    rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_mact_opport_mode_get, (unit,&opport));
    if (rv < 0) {
        cli_out(errMsgOpport);
        return rv;
    }

    if (opport == 0) {
        cli_out("L2 Opportunistic Learning: Opportunistic learn mode is OFF\n");
    }
    else {
        cli_out("L2 Opportunistic Learning: Opportunistic learn mode is ON\n");
    }

    rv = bcm_switch_control_get(unit,bcmSwitchL2LearnMode,&learn_mode);
    if (rv < 0) {
        cli_out(errMsgLearnMode);
        return rv;
    }

    switch (learn_mode) {
    case BCM_L2_INGRESS_CENT:
        sal_sprintf(learn_mode_string,"Ingress Centralized");
    break;
    case BCM_L2_INGRESS_DIST:
        sal_sprintf(learn_mode_string,"Ingress Distributed");
    break;
    case BCM_L2_EGRESS_DIST:
        sal_sprintf(learn_mode_string,"Egress Distributed");
    break;
    case BCM_L2_EGRESS_CENT:
        sal_sprintf(learn_mode_string,"Egress Centralized");
    break;
    case BCM_L2_EGRESS_INDEPENDENT:
        sal_sprintf(learn_mode_string,"Egress Independent");
    break;
    case BCM_L2_LEARN_CPU:
        sal_sprintf(learn_mode_string,"CPU Learning");
    break;
    case BCM_L2_LEARN_DISABLE:
        sal_sprintf(learn_mode_string,"Learning Disabled");
    break;
    }

    cli_out("L2 Learning Mode: %s \n",learn_mode_string);

    rv = bcm_l2_age_timer_get(unit, &age_seconds);
    if (rv < 0) {
        cli_out(errMsgAge);
        return rv;
    }

    if (age_seconds == 0) {
        cli_out("L2 Aging: Aging is OFF \n");
    }
    else {
        cli_out("L2 Aging: Aging time is %d seconds \n",age_seconds);
    }
    return CMD_OK;
}

STATIC cmd_result_t
_cmd_dpp_l2_add(int unit, args_t *args)
{
    cmd_result_t   retCode;
    int            rv;
    parse_table_t  pt;
    pbmp_t         pbmp;
    bcm_l2_addr_t  l2addr;
    int            port = BCM_GPORT_INVALID;
    bcm_gport_t    g_port;

    /* Parse command option arguments */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC, 0, &_l2_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &_l2_vlan, NULL);
    parse_table_add(&pt, "Module", PQ_DFL|PQ_INT, 0, &_l2_modid, NULL);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP, (void *)(0),
                    &_l2_pbmp, NULL);
    parse_table_add(&pt, "Trunk", PQ_DFL|PQ_BOOL, 0, &_l2_trunk, NULL);
    parse_table_add(&pt, "TrunkGroupID", PQ_DFL|PQ_INT, 0, &_l2_tgid, NULL);
    parse_table_add(&pt, "DiscardSource", PQ_DFL|PQ_BOOL, 0, &_l2_dsrc, NULL);
    parse_table_add(&pt, "DiscardDest",  PQ_DFL|PQ_BOOL, 0, &_l2_ddest, NULL);
    parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL, 0, &_l2_static, NULL);

    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    pbmp = _l2_pbmp;
    if (BCM_PBMP_IS_NULL(pbmp) && !_l2_trunk) {
        cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    /*
     * If we are setting the range, the MAC address is incremented by
     * 1 for each port.
     */
    if (!_l2_trunk) {
    PBMP_ITER(pbmp, port) {
        bcm_l2_addr_t_init(&l2addr, _l2_macaddr, _l2_vlan);

        /* Configure L2 addr flags */
        if (_l2_static) {
            l2addr.flags |= BCM_L2_STATIC;
        }
        if (_l2_ddest) {
            l2addr.flags |= BCM_L2_DISCARD_DST;
        }
        if (_l2_dsrc) {
            l2addr.flags |= BCM_L2_DISCARD_SRC;
        }
        if (_l2_trunk) {
            l2addr.flags |= BCM_L2_TRUNK_MEMBER;
        }

        if (_l2_modid>0) {
            BCM_GPORT_MODPORT_SET(g_port, _l2_modid, port);
            l2addr.port  = g_port;
        }
        else {
            l2addr.port  = port;
        }

        l2addr.modid = _l2_modid;
        l2addr.tgid  = _l2_tgid;

        if (bsl_check(bslLayerAppl, bslSourceShell, bslSeverityVerbose, unit) &&
            bsl_check(bslLayerAppl, bslSourceL2, bslSeverityNormal, unit)) {
            dump_l2_addr(unit, "ADD: ", &l2addr);
        }

        rv = bcm_l2_addr_add(unit, &l2addr);
        if (BCM_FAILURE(rv)) {
            cli_out("%s Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        /* Set up for next call */
        increment_macaddr(_l2_macaddr, 1);
        }
    } else {
        bcm_l2_addr_t_init(&l2addr, _l2_macaddr, _l2_vlan);

        /* Configure L2 addr flags */
        if (_l2_static) {
            l2addr.flags |= BCM_L2_STATIC;
        }
        if (_l2_ddest) {
            l2addr.flags |= BCM_L2_DISCARD_DST;
        }
        if (_l2_dsrc) {
            l2addr.flags |= BCM_L2_DISCARD_SRC;
        }
        if (_l2_trunk) {
            l2addr.flags |= BCM_L2_TRUNK_MEMBER;
        }

        if (_l2_modid>0) {
            BCM_GPORT_MODPORT_SET(g_port, _l2_modid, port);
            l2addr.port  = g_port;
        }
        else {
            l2addr.port  = port;
        }

        l2addr.modid = _l2_modid;
        l2addr.tgid  = _l2_tgid;

        if (bsl_check(bslLayerAppl, bslSourceShell, bslSeverityVerbose, unit) &&
            bsl_check(bslLayerAppl, bslSourceL2, bslSeverityNormal, unit)) {
            dump_l2_addr(unit, "ADD: ", &l2addr);
        }

        rv = bcm_l2_addr_add(unit, &l2addr);
        if (BCM_FAILURE(rv)) {
            cli_out("%s Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/*
 * When soc feature flush_non_blocking_when_all_actions_delete was enabled,  
 * flush DB should be cleared. Otherwise these rules will affect other operation.
 */
STATIC cmd_result_t
dpp_clear_flush_db(int unit)
{
    int            rv, index, is_wait_success = FALSE;
    uint32         nof_matched_entries, waiting_time_in_ms;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "flush_non_blocking_when_all_actions_delete", 0)) {
        for (index = 0; index < 1000; ++index) {
            /* When flush check is finished, flush db will be cleared. */
            rv = _arad_pp_frwrd_lem_traverse_check_flush_is_done(unit, &nof_matched_entries, &waiting_time_in_ms, &is_wait_success);
            if (BCM_FAILURE(rv)) {
                cli_out("Check flush is done error: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (is_wait_success) {
                break;
            }

            rv = soc_sand_os_task_delay_milisec(waiting_time_in_ms);
            if (BCM_FAILURE(rv)) {
                cli_out("Delay error: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        if (is_wait_success == FALSE) {
            cli_out("Waiting failed.\n");
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/* CLEAR command options to delete MAC entries 'by' these arguments */
#define DELETE_BY_MAC     0x00000001
#define DELETE_BY_VLAN    0x00000002
#define DELETE_BY_MODID   0x00000004
#define DELETE_BY_PORT    0x00000008
#define DELETE_BY_TGID    0x00000010

STATIC cmd_result_t
_cmd_dpp_l2_clear(int unit, args_t *args)
{
    cmd_result_t   retCode;
    int            rv;
    parse_table_t  pt;
    char           *static_str;
    bcm_mac_t      macaddr;
    int            vlan = -1, modid = -1, port = -1, tgid = -1;
    int            mac_static = TRUE;
    int            delete_by = 0;
    char           *option;

    ENET_SET_MACADDR(macaddr, _soc_mac_all_zeroes);

    if (!ARG_CNT(args)) {
        /*
         * Use current global L2 argument values if none arguments given
         */
        ENET_SET_MACADDR(macaddr, _l2_macaddr);
        vlan  = _l2_vlan;
        modid = _l2_modid;
        port  = _l2_port;
        tgid  = _l2_tgid;
        mac_static = _l2_static;

    } else {
        option = args->a_argv[2]; /* The position of the first parameter of l2 clear command */
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(option, "all", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
            /* Deleting all mac table using bcm_l2_replace */
            int flags = BCM_L2_REPLACE_MATCH_STATIC|BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_NO_CALLBACKS|
                BCM_L2_REPLACE_IGNORE_DISCARD_SRC|BCM_L2_REPLACE_IGNORE_DES_HIT;
            bcm_l2_addr_t dummy_addr;
            sal_mac_addr_t dummy_mac = {0,0,0,0,0,0};
            bcm_l2_addr_t_init(&dummy_addr, dummy_mac, 0);
            cli_out("%s: Deleting whole l2 mac table\n",ARG_CMD(args));
            rv = bcm_l2_replace(unit, flags, &dummy_addr, 0, 0, 0);
            if (BCM_FAILURE(rv)) {
                cli_out("%s Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            option = ARG_GET(args); /* This is done to avoid warning print about excess arguments ignored*/
            return dpp_clear_flush_db(unit);
        }
        else {
            /* Parse command option arguments */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC, 0, &macaddr, NULL);
            parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &vlan, NULL);
            parse_table_add(&pt, "Module", PQ_DFL|PQ_INT, 0, &modid, NULL);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &port, NULL);
            parse_table_add(&pt, "TrunkGroupID", PQ_DFL|PQ_INT, 0, &tgid, NULL);
            parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL, 0, &mac_static, NULL);

            if (!parseEndOk(args, &pt, &retCode)) {
                return retCode;
            }
        }
    }
    
    /*
     * Determine which arguments were supplied
     */
    if (ENET_CMP_MACADDR(macaddr, _soc_mac_all_zeroes)) {
        delete_by |= DELETE_BY_MAC;
    }
    if (vlan >= 0) {
        delete_by |= DELETE_BY_VLAN;
    }
    if (modid >=0) {
        delete_by |= DELETE_BY_MODID;
    }
    if (port >= 0) {
        delete_by |= DELETE_BY_PORT;
    }
    if (tgid >= 0) {
        delete_by |= DELETE_BY_TGID;
    }

    static_str = mac_static ? "static and non-static" : "non-static";
    mac_static = mac_static ? BCM_L2_DELETE_STATIC : 0;

    /* Delete 'by' */
    switch (delete_by) {
    case DELETE_BY_MAC:
        cli_out("%s: Deleting %s addresses by MAC\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_mac(unit, macaddr, mac_static);
        break;

    case DELETE_BY_VLAN:
        cli_out("%s: Deleting %s addresses by VLAN\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_vlan(unit, vlan, mac_static);
        break;
        
    case DELETE_BY_MAC | DELETE_BY_VLAN:
        cli_out("%s: Deleting an address by MAC and VLAN\n",
                ARG_CMD(args));
        rv = bcm_l2_addr_delete(unit, macaddr, vlan);
        break;

    case DELETE_BY_PORT:
        cli_out("%s: Deleting %s addresses by port, local module ID\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_port(unit, -1, port, mac_static);
        break;

    case DELETE_BY_PORT | DELETE_BY_MODID:
        cli_out("%s: Deleting %s addresses by module/port\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_port(unit, modid, port, mac_static);
        break;

    case DELETE_BY_TGID:
        cli_out("%s: Deleting %s addresses by trunk ID\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_trunk(unit, tgid, mac_static);
        break;

    case DELETE_BY_MAC | DELETE_BY_PORT:
        cli_out("%s: Deleting %s addresses by MAC and port\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_mac_port(unit, macaddr, -1, port,
                                            mac_static);
        break;

    case DELETE_BY_MAC | DELETE_BY_PORT | DELETE_BY_MODID:
        cli_out("%s: Deleting %s addresses by MAC and module/port\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_mac_port(unit, macaddr, modid, port,
                                            mac_static);
        break;

    case DELETE_BY_VLAN | DELETE_BY_PORT:
        cli_out("%s: Deleting %s addresses by VLAN and port\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_vlan_port(unit, vlan, -1, port,
                                             mac_static);
        break;

    case DELETE_BY_VLAN | DELETE_BY_PORT | DELETE_BY_MODID:
        cli_out("%s: Deleting %s addresses by VLAN and module/port\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_vlan_port(unit, vlan, modid, port,
                                             mac_static);
        break;

    case DELETE_BY_VLAN | DELETE_BY_TGID:
        cli_out("%s: Deleting %s addresses by VLAN and trunk ID\n",
                ARG_CMD(args), static_str);
        rv = bcm_l2_addr_delete_by_vlan_trunk(unit, vlan, tgid, mac_static);
        break;

    default:
        cli_out("%s: Unknown argument combination\n", ARG_CMD(args));
        return CMD_USAGE;
        break;
    }

    if (BCM_FAILURE(rv)) {
        cli_out("%s Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return dpp_clear_flush_db(unit);
}


STATIC cmd_result_t
_cmd_dpp_l2_del(int unit, args_t *args)
{
    cmd_result_t   retCode;
    int            rv;
    parse_table_t  pt;
    bcm_l2_addr_t  l2addr;
    int            idx;
    int            count = 1;

    /* Parse command option arguments */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC, 0, &_l2_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &_l2_vlan, NULL);
    parse_table_add(&pt, "Count", PQ_DFL|PQ_INT, (void *)(1), &count, NULL);
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    for (idx = 0; idx < count; idx++) {
        rv = bcm_l2_addr_get(unit, _l2_macaddr, _l2_vlan, &l2addr);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (bsl_check(bslLayerAppl, bslSourceShell, bslSeverityVerbose, unit) &&
            bsl_check(bslLayerAppl, bslSourceL2, bslSeverityNormal, unit)) {
            dump_l2_addr(unit, "DEL: ", &l2addr);
        }

        rv = bcm_l2_addr_delete(unit, _l2_macaddr, _l2_vlan);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        increment_macaddr(_l2_macaddr, 1);
    }

    return CMD_OK;
}


STATIC cmd_result_t
_cmd_dpp_l2_init(int unit, args_t *args)
{
    int  rv;

    if (ARG_CNT(args) > 0) {
        return CMD_USAGE;
    }

    if (BCM_FAILURE(rv = bcm_l2_init(unit))) {
        cli_out("%s Error: %s\n", ARG_CMD(args), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/* CB function to print L2 Entry */
int dpp_l2addr_dump(int unit, bcm_l2_addr_t *l2addr, void *user_data)
{
    SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;
    uint32  soc_sand_dev_id;
    int  rv;
    uint8 found;
    SOC_PPC_FRWRD_DECISION_INFO *info;

    dump_l2_addr(unit, "", l2addr);
    if (*(int*)(user_data)) {
          soc_sand_dev_id = (unit);
          rv = _bcm_petra_l2_to_petra_entry(unit, l2addr, TRUE, &l2_entry_key, &l2_entry_val); 
          if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
          }
          rv = soc_ppd_frwrd_mact_entry_get(soc_sand_dev_id,&l2_entry_key,&l2_entry_val,&found);
          if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
          }     
          cli_out("                      ");
          if (l2_entry_key.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
          {
              cli_out("FID=%d ", l2_entry_key.key_val.mac.fid);
          }
          else if (l2_entry_key.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
          {
              cli_out("FID=%d ", l2_entry_key.key_val.ipv4_mc.fid);
          }
          cli_out("SA_Drp=%s ", (l2_entry_val.frwrd_info.drop_when_sa_is_known)?"V":"X");
          info=&l2_entry_val.frwrd_info.forward_decision;
          cli_out("Destination_Type=%s ", SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type));
          if (info->type != SOC_PPC_FRWRD_DECISION_TYPE_DROP)
          {
            cli_out("Val=%d ",info->dest_id);
          }
          switch(info->type)
          {
          case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
          case SOC_PPC_FRWRD_DECISION_TYPE_MC:
            cli_out("Dest Type=%s ",
                    "EEI"
                    );
            switch(info->additional_info.eei.type)
            {
              case SOC_PPC_EEI_TYPE_EMPTY:
                cli_out("Additional_Info=%s ",
                        "EMPTY"
                        );
                break;
              case SOC_PPC_EEI_TYPE_MIM:
                cli_out("Additional_Info=%s %u ",
                        " ISID", info->additional_info.eei.val.isid
                        );
                break;
              case SOC_PPC_EEI_TYPE_MPLS:
                if (info->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_PUSH)
                {
                  cli_out("Push_L=%u Pr=%u ",
                          info->additional_info.eei.val.mpls_command.label,
                          info->additional_info.eei.val.mpls_command.push_profile
                          );
                }
                else if (info->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_SWAP)
                {
                  cli_out("Swap_L=%u ",
                          info->additional_info.eei.val.mpls_command.label
                          );
                }
                else /*POP*/
                {
                  cli_out("%s ",
                          SOC_PPC_MPLS_COMMAND_TYPE_to_string(info->additional_info.eei.val.mpls_command.command)
                          );
                }
                break;
              case SOC_PPC_EEI_TYPE_TRILL:
                cli_out("%s%u %s ",
                        "TRILL_Dest=",
                        info->additional_info.eei.val.trill_dest.dest_nick,
                        (info->additional_info.eei.val.trill_dest.is_multicast)?"MC":"UC"
                        );
                break;
              default:
                break;
            }
            break;
          case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
          case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
          case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
            cli_out("%s",
                    "OUTLIF="
                    );
            cli_out("%s %u ",
                    SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(info->additional_info.outlif.type),
                    info->additional_info.outlif.val
                    );
            break;
          case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
            cli_out("%s ",
                    "TRAP"
                    );
            cli_out("%s (BCM API: %s) %u ",
                    SOC_PPC_TRAP_CODE_to_string(info->additional_info.trap_info.action_profile.trap_code),
                    SOC_PPC_TRAP_CODE_to_api_string(info->additional_info.trap_info.action_profile.trap_code),
                    info->additional_info.trap_info.action_profile.frwrd_action_strength
                    );
          break;
          default:
            break;
          }
          cli_out("\n\r");
  } 

    return BCM_E_NONE;
}

STATIC cmd_result_t
_cmd_dpp_l2_show(int unit, args_t *args)
{
    cmd_result_t   ret_code;
    int            rv;
    bcm_mac_t      mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    int            vid = 0;
    int            max_count = 0;
    int            debug = 0;
    int            sw = 0;

    if (ARG_CNT(args) > 0) {
        parse_table_t  pt;
        
        /* Parse option */
        parse_table_init(0, &pt);
        parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC, 0, &mac, NULL);
        parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &vid, NULL);
        parse_table_add(&pt, "COUNT", PQ_DFL | PQ_INT, 0, &max_count,  NULL);
        parse_table_add(&pt, "debug", PQ_DFL | PQ_INT, 0, &debug,  NULL);
        parse_table_add(&pt, "sw", PQ_DFL | PQ_INT, 0, &sw,  NULL);
        if (!parseEndOk(args, &pt, &ret_code)) {
            return ret_code;
        }
    }

    if (sw) {
        rv = _bcm_petra_l2_sw_traverse(unit, dpp_l2addr_dump, &debug);
    }
    else
    {
        rv = bcm_l2_traverse(unit, dpp_l2addr_dump, &debug);
    }
    
    if (rv < 0) {
        cli_out("%s: ERROR: bcm_l2_traverse failed %s\n",
                ARG_CMD(args), bcm_errmsg(rv));
    }

    return CMD_OK;
}

cmd_result_t
cmd_dpp_l2(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              _cmd_dpp_l2_list, COUNTOF(_cmd_dpp_l2_list));
}


/*
 * L2 Age
 *
 * Note:
 *     The CLI to manipulate aging is set in a command group different
 *     from L2.  This maintains the same CLI command menu as the
 *     current XGS CLI.
 */
char cmd_dpp_age_usage[] =
    "Parameters:  [<seconds>]\n\t"
    "   Set the L2 age timer to the indicated number of seconds.\n\t"
    "   With no parameter, displays current value.\n\t"
    "   Setting to 0 disables L2 aging.\n";

cmd_result_t
cmd_dpp_age(int unit, args_t *args)
{
    int  seconds;
    int  rv;

    if (!ARG_CNT(args)) {    /* Display settings */
        rv = bcm_l2_age_timer_get(unit, &seconds);
        if (BCM_FAILURE(rv)) {
            cli_out("%s Error: could not get age time: %s\n",
                    ARG_CMD(args), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        cli_out("Current age timer is %d %s\n",
                seconds, seconds ? "seconds" : "(disabled)");

        return CMD_OK;
    }

    seconds = sal_ctoi(ARG_GET(args), 0);

    rv = bcm_l2_age_timer_set(unit, seconds);
    if (BCM_FAILURE(rv)) {
        cli_out("%s Error: could not set age time: %s\n",
                ARG_CMD(args), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("Set age timer to %d %s\n",
            seconds, seconds ? "seconds" : "(disabled)");

    return CMD_OK;
}

