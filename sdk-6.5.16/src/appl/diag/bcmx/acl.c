/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Access Control Lists (ACL) related CLI commands.
 */

#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/acl/acl.h>
#include <appl/diag/diag.h>
#include "../../acl/acl_util.h"

#include <bcmx/lplist.h>
#include <bcmx/lport.h>
#include <bcmx/port.h>
#include <bcm/error.h>

#include "bcmx.h"

#if defined(INCLUDE_ACL)

#define BCMA_ACL_NUMB_LISTS  16 /* Max. number of lists in diag. code. */
#define BCMA_ACL_NUMB_RULES  16 /* Max. number of rules in each list.  */

/*
 * Macro:
 *     ACL_CHECK_RETURN
 * Purpose:
 *     Check the return value from an API call. Output either a failed
 *     message or okay along with the function name.
 */
#define ACL_CHECK_RETURN(retval, funct_name) \
    if ((retval) != BCM_E_NONE) { \
        cli_out("%s() failed: %s\n", (funct_name), bcm_errmsg(retval)); \
        return CMD_FAIL; \
    } \
    cli_out("%s() okay\n", (funct_name));

/*
 * Macro:
 *     ACL_GET_NUMB
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define ACL_GET_NUMB(numb, str, args) \
    if (((str) = ARG_GET(args)) == NULL) { \
        return CMD_USAGE; \
    } \
    (numb) = parse_integer(str);

/*
 * Macro:
 *     ACL_GET_PORT
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define ACL_GET_PORT(_unit, _port, _str, _args)                   \
    if (((_str) = ARG_GET(_args)) == NULL) {                     \
        return CMD_USAGE;                                        \
    }                                                            \
    if (parse_port((_unit), (_str), &(_port)) < 0) {             \
       ACL_ERR(("ERROR: invalid port string: \"%s\"\n", (_str))); \
       return CMD_FAIL;                                          \
    }

#define MAC_IS_CLEAR(mac)                                                 \
((mac)[5] == 0 && (mac)[4] == 0 && (mac)[3] == 0 && (mac)[2] == 0 &&      \
 (mac)[1] == 0 && (mac)[0] == 0  ? TRUE : FALSE)

#define IP6_IS_CLEAR(ip6)                                                 \
((ip6)[15] == 0 && (ip6)[14] == 0 && (ip6)[13] == 0 && (ip6)[12] == 0 &&  \
 (ip6)[11] == 0 && (ip6)[10] == 0 && (ip6)[ 9] == 0 && (ip6)[ 8] == 0 &&  \
 (ip6)[ 7] == 0 && (ip6)[ 6] == 0 && (ip6)[ 5] == 0 && (ip6)[ 4] == 0 &&  \
 (ip6)[ 3] == 0 && (ip6)[ 2] == 0 && (ip6)[ 1] == 0 && (ip6)[ 0] == 0     \
  ? TRUE : FALSE)


/*
 * Marker for last element in qualification table 
 */
#define ACL_TABLE_END_STR "tbl_end"

#define ACL_STAT_STR_SZ 256

/*
 * local typedefs
 */

typedef struct cmd_acl_list_s {
    bcma_acl_t          list;
    bcma_acl_rule_t     *rules[BCMA_ACL_NUMB_RULES];
} cmd_acl_list_t;


cmd_acl_list_t *acl_lists[BCMA_ACL_NUMB_LISTS];


/*
 * local function prototypes
 */
cmd_result_t bcmx_cmd_aclcreate(int unit, args_t *args) ;
cmd_result_t bcmx_cmd_acldetach(int unit, args_t *args) ;
cmd_result_t bcmx_cmd_aclinit(int unit, args_t *args);
cmd_result_t bcmx_cmd_aclinstall(int unit, args_t *args);
cmd_result_t bcmx_cmd_acluninstall(int unit, args_t *args);

#ifdef BROADCOM_DEBUG
cmd_result_t bcmx_cmd_aclshow(int unit, args_t *args) ;
#endif /*BROADCOM_DEBUG*/

cmd_result_t bcmx_cmd_acl_list(int unit, args_t *args);
cmd_result_t bcmx_cmd_acl_list_create(int unit, args_t *args);
cmd_result_t bcmx_cmd_acl_list_destroy(int unit, args_t *args);

cmd_result_t bcmx_cmd_aclrule(int unit, args_t *args);

STATIC void _cmd_acl_list_destroy(cmd_acl_list_t *list);
STATIC bcma_acl_t *_cmd_acl_list_alloc(bcma_acl_list_id_t list_id);
STATIC int _bcmx_acl_rule_id_get(void);
int _bcmx_acl_rule_add(bcma_acl_list_id_t list_id, bcma_acl_rule_t *rule);
STATIC bcma_acl_rule_t * _bcmx_acl_rule_alloc(void);

#define BCMA_ACL_LIST_NUMB 4

static int acl_init_flg = FALSE;
static int list_id_cur, list_id_max;
static int rule_id_max;

char bcmx_cmd_acl_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: [acl_id] <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
"Parameters: [acl_id] <cmd> <opt>\n"
" Where <cmd> is:\n"
"\tlist  create  <list_id> <user-port-list>\t- Create an ACL on a set of ports\n"
"\t      destroy id=<list_id>\n"
"\trule {Permit=<T/F>|Deny=<T/F>|Log=<T/F>}\n"
"\t       [RedirPort=<user-port>] [MirrorPort=<user-port>]\n"
"\t       [SrcMac=<val>] [DstMac=<val>]\n"
"\t       [SrcIp=<val>]  [SrcIpMask=<val>]  [DstIp=<val>]  [DstIpMask=<val>]\n"
"\t       [SrcIp6=<val>] [SrcIp6Mask=<val>] [DstIp6=<val>] [DstIp6Mask=<val>]\n"
"\t       [SrcPortMIn=<val>] [SrcPortMAx=<val>]\n"
"\t       [DstPortMIn=<val>] [DstPortMAx=<val>]\n"
"\t       [VlanMIn=<id>] [VlanMAx=<id>\n"
"\t       [IpProtocol=<protocol>] [EtherType=<type>\n"
"\tdetach\n"
"\tinit\n"
"\tinstall\n"
"\tuninstall\n"
"\tshow\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;

static cmd_t acl_cmd_list[] = {
    {"Init", bcmx_cmd_aclinit,
     "acl init",
     "Initialize ACL"},

    {"Detach",  bcmx_cmd_acldetach,
     "acl detach",
     "Detach ACL"},

    {"Rule",   bcmx_cmd_aclrule,
     "acl rule {Permit|Deny|Redirect|Log}",
     "Specify an access control list rule"},

    {"List", bcmx_cmd_acl_list,
     "acl list {create | destroy}",
     "Create or destroy access control list"},

#ifdef BROADCOM_DEBUG
    {"Show", bcmx_cmd_aclshow,
     "acl show",
     "Show an access control list" },
#endif /* BROADCOM_DEBUG */

    {"Install", bcmx_cmd_aclinstall,
     "acl install",
     "Install the ACL lists in harware"},

    {"Uninstall", bcmx_cmd_acluninstall,
     "acl uninstall",
     "Remove the ACL lists from harware"}
};

static cmd_t acl_list_cmd_list[] = {
    {"Create", bcmx_cmd_acl_list_create,
     "acl list create",
     "Create an ACL list" },

     {"Destroy",  bcmx_cmd_acl_list_destroy,
     "acl list destroy",
     "destroy an ACL list" }
};

static
char *bcma_acl_ip_protocol_text[256] = {
    "0",     "ICMP",  "IGMP",  "GGP",     "IP",     "ST",    "TCP",  "UCL",
    "EGP",    "IGP",    "10",   "11",     "PUP", "ARGUS",  "EMCON", "XNET",
    "CHAOS",  "UDP",   "MUX",   "19",     "HMP",   "PRM",     "22",   "23",
    "24",   "LEAF1", "LEAF2",  "RDP",     "IRTP",   "29", "NETBLT",   "31",
    "32",     "SEP",   "3PC", "IDRP",     "XTP",   "DDP",     "38", "TP++",
    "IL",     "SIP",  "SDRP",   "43",     "44",   "IDRP",   "RSVP",  "GRE",
    "48",      "49",    "50",   "51",     "52",     "53",     "54",   "55",
};

/* Section: top level commands */

/*
 * Function:
 *      bcmx_cmd_acl
 * Purpose:
 *      Access Control List CLI interface
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to command line arguments      
 * Returns:
 *    CMD_OK
 */
cmd_result_t
bcmx_cmd_acl(int unit, args_t *args) {
    char            *initial_cmd = ARG_CMD(args);
    char            *key;
    cmd_t           *cmd;
    char            *param;
    cmd_result_t    retval = CMD_OK;

    if ((key = ARG_GET(args)) == NULL) {
        sal_printf("%s:  Subcommand required\n", initial_cmd);
        return CMD_USAGE;
    }

    if (isint(key)) {
        list_id_cur = parse_integer(key);
        if ((key = ARG_GET(args)) == NULL) {
            sal_printf("%s:  Subcommand required\n", initial_cmd);
            return CMD_USAGE;
        }
    }

    cmd = (cmd_t *) parse_lookup(key, acl_cmd_list, sizeof(cmd_t), 
                                 COUNTOF(acl_cmd_list));

    if (cmd == NULL)
    {
        sal_printf("%s: Unknown subcommand %s\n", initial_cmd, key);
        return CMD_USAGE;
    }

    /* If next param is "?", just display help for command */    
    param = ARG_CUR(args);
    if (param != NULL) {
        if (sal_strcmp(param, "?") == 0)
        {
            ARG_GET(args);    /* Consume "?" */
            if ((cmd->c_usage != NULL) &&
                (soc_property_get(unit, spn_HELP_CLI_ENABLE, 1))) {
                sal_printf("Usage:  %s\n", cmd->c_usage);
            }
            if ((cmd->c_help != NULL) && 
                (soc_property_get(unit, spn_HELP_CLI_ENABLE, 1))){
                sal_printf("Help :  %s\n", cmd->c_help);
            }
            return retval;
        }
    }

    return cmd->c_f(unit, args);
}


/* Section: 2nd level commands */

/*
 * Function: bcmx_cmd_acldetach
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_acldetach(int unit, args_t *args) {
    int                 retval;
    int                 idx;

    /* Do the actual ACL detach. */
    retval = bcma_acl_detach();
    ACL_CHECK_RETURN(retval, "bcma_acl_detach");

    /* Get rid of any locally allocated lists. */
    for (idx = 0; idx < BCMA_ACL_NUMB_LISTS; idx++) {
        if (acl_lists[idx] != NULL) {
            _cmd_acl_list_destroy(acl_lists[idx]);
            acl_lists[idx] = NULL;
        }
    }

    acl_init_flg = FALSE;

    return CMD_OK;
}

/*
 * Function: _cmd_acl_list_destroy
 *
 * Purpose:
 *     free any locally allocated ACL lists, including all rules in list.
 *
 * Parameters:
 *
 * Returns:
 */
STATIC void
_cmd_acl_list_destroy(cmd_acl_list_t *cmd_list)
{
    int                 idx;

    assert(cmd_list != NULL);

    for (idx = 0; idx < BCMA_ACL_NUMB_RULES; idx++) {
        if (cmd_list->rules[idx] != NULL) {
            bcma_acl_rule_remove(cmd_list->list.list_id, cmd_list->rules[idx]->rule_id);
            sal_free(cmd_list->rules[idx]);
            cmd_list->rules[idx] = NULL;
        }
    }
    sal_free(cmd_list);
}

/*
 * Function: bcmx_cmd_aclinit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_aclinit(int unit, args_t *args) {
    int                idx;

    /* Clean up first, if previously initialized. */
    if (acl_init_flg == TRUE) {
        /* Get rid of any locally allocated lists. */
        for (idx = 0; idx < BCMA_ACL_NUMB_LISTS; idx++) {
            if (acl_lists[idx] != NULL) {
                _cmd_acl_list_destroy(acl_lists[idx]);
                acl_lists[idx] = NULL;
            }
        }
    }

    /* Zero out local list data. */
    sal_memset(acl_lists, 0, sizeof(cmd_acl_list_t*) * BCMA_ACL_NUMB_LISTS);
    list_id_cur = list_id_max = 1;
    rule_id_max = 1;

    /* Initialize the ACL module itself. */ 
    bcma_acl_init();
    acl_init_flg = TRUE;
    return CMD_OK;
}

/*
 * Function: bcmx_cmd_aclinstall
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_aclinstall(int unit, args_t *args) {
    int                 retval;

    retval = bcma_acl_install();
    ACL_CHECK_RETURN(retval, "bcma_acl_install");

    return CMD_OK;
}
/*
 * Function: bcmx_cmd_acluninstall
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_acluninstall(int unit, args_t *args) {
    int                 retval;

    retval = bcma_acl_uninstall();
    ACL_CHECK_RETURN(retval, "bcma_acl_install");

    return CMD_OK;
}


#ifdef BROADCOM_DEBUG
/*
 * Function: bcmx_cmd_aclshow
 *
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_aclshow(int unit, args_t *args) {
    int                 retval;
    char                *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* If no List ID specified, show all lists. */
        retval = bcma_acl_show();
        ACL_CHECK_RETURN(retval, "bcma_acl_show");
        return CMD_OK;
    }

    /* Use the command line List ID. */
    if (!isint(subcmd)) {
        return CMD_USAGE;
    }

    retval = bcma_acl_list_show(parse_integer(subcmd));
    ACL_CHECK_RETURN(retval, "bcma_acl_list_show");

    return CMD_OK;
}
#endif /*BROADCOM_DEBUG*/


/* Section: List Commands {Create | Destroy} */
cmd_result_t
bcmx_cmd_acl_list(int unit, args_t *args) {
    return subcommand_execute(unit, args, 
                              acl_list_cmd_list, 
                              COUNTOF(acl_list_cmd_list));
}

/*
 * Function: bcmx_cmd_acl_list_create
 *
 * Purpose:
 *    CLI handler to create an ACL list.
 *
 * Parameters:
 *
 * Returns:
 *     CMD_OK - Success
 *     CMD_FAIL - could not initialize port list or list add failure.
 *     CMD_USAGE - improper CLI
 */
cmd_result_t
bcmx_cmd_acl_list_create(int unit, args_t *args) {
    bcma_acl_list_id_t  list_id = list_id_max + 1;
    bcma_acl_t          *list_p;
    char                *str;

    /* Read the List ID. */
    str = ARG_GET(args);
    if (str) {
        list_id = parse_integer(str);
    } else {
        return CMD_USAGE;
    }

    list_p = _cmd_acl_list_alloc(list_id);
    if (list_p == NULL) {
        cli_out("ACL: list ID=%d allocation failure\n", list_id);
        return CMD_FAIL;
    }

    /* Read the user port list. */
    str = ARG_GET(args);
    if (str) {
        if (BCM_FAILURE(bcmx_lplist_parse(&list_p->lplist, str))) {
            cli_out("Can't parse UserPortList \"%s\"\n", str);
            return CMD_USAGE;
        }
        /* Read the priority */
        str = ARG_GET(args);
        list_p->prio = (str) ? parse_integer(str) : 0;
    } else {
       bcmx_port_lplist_populate(&list_p->lplist, BCMX_PORT_LP_ALL);
    }

    list_id_cur    = list_id;
    list_p->list_id    = list_id;

    if (BCM_FAILURE(bcma_acl_add(list_p))) {
        return CMD_FAIL;
    }

    cli_out("ACL: list ID=%d created\n", list_id);

    return CMD_OK;
}

/*
 * Function: _cmd_acl_list_alloc
 *
 * Purpose:
 *     Allocate a command list struct from local array
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     pointer to bcma_acl_t list
 *     or
 *     NULL on allocation failure
 */
STATIC bcma_acl_t *
_cmd_acl_list_alloc(bcma_acl_list_id_t list_id)
{
    int                 idx;
    cmd_acl_list_t      *list = NULL;

    /* Find an unused slot in acl_list array. */
    for (idx = 0; idx < BCMA_ACL_NUMB_LISTS; idx++) {
        if (acl_lists[idx] == NULL) {
            /* Allocate the cmd_list struct. */
            list = sal_alloc(sizeof(cmd_acl_list_t), "CMD ACL");
            if (list == NULL) {
                return NULL;
            }
            /* Put it in acl_list array. */
            acl_lists[idx] = list;

            /* Zero out cmd_list struct. */
            sal_memset(list, 0, sizeof(cmd_acl_list_t));

            bcmx_lplist_init(&list->list.lplist, 0, 0);
	    list->list.list_id = list_id;

            /* Return pointer to bcma_acl_t list. */
            return &list->list;
	}
    }

    /* Return pointer to bcma_acl_t list. */
    return NULL;
}

cmd_result_t
bcmx_cmd_acl_list_destroy(int unit, args_t *args) {
    parse_table_t       pt;
    cmd_result_t     retval;
    bcma_acl_list_id_t   list_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "id", PQ_DFL | PQ_INT, 0,
                    (void *)&list_id, 0);

    if(!parseEndOk(args, &pt, &retval)) {
        return CMD_USAGE;
    }

    if (BCM_FAILURE(bcma_acl_remove(list_id))) {
        cli_out("ACL: failed to destroy list ID=%d \n", list_id);
        return CMD_FAIL;
    }

    cli_out("ACL: List ID=%d destroyed\n", list_id);

    return CMD_OK;
}

/* Section: rule Commands {Create | Destroy} */

STATIC int
_bcmx_acl_rule_id_get(void) {
    int                 rule_id; /* output */

    rule_id_max++;
    rule_id     = rule_id_max;
    
    return rule_id;
}

/*
 * Function: bcmx_cmd_aclrule
 *
 * Purpose:
 *    Create a list rule.
 *
 * Parameters:
 *
 * Returns:
 */
cmd_result_t
bcmx_cmd_aclrule(int unit, args_t *args) {
    bcma_acl_rule_t     *rule;
    parse_table_t       pt;
    cmd_result_t        retval;
    uint32              permit_flag=0, deny_flag=0, log_flag=0;
    char                *redir_port_str = NULL;
    char                *mirror_port_str = NULL;
    bcmx_lport_t        lport;
    bcmx_uport_t        uport;
    uint32              vlan_min = 0, vlan_max = 0;
    uint32              ethertype = 0;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META_U(unit,
                          "BEGIN bcmx_cmd_aclrule()\n")));

    rule = _bcmx_acl_rule_alloc();
    if (rule == NULL) {
        return CMD_FAIL;
    }

    rule->rule_id             = _bcmx_acl_rule_id_get() ;
    rule->ip_protocol         = BCMA_ACL_IPPROTO_ANY;
    rule->actions.redir_port  = -1;
    rule->actions.mirror_port = -1;

    parse_table_init(unit, &pt);

    /* Actions */
    parse_table_add(&pt, "Permit", PQ_DFL | PQ_NO_EQ_OPT | PQ_BOOL, 0,
                    (void *)&permit_flag, 0);
    parse_table_add(&pt, "Deny",   PQ_DFL | PQ_NO_EQ_OPT | PQ_BOOL, 0,
                    (void *)&deny_flag, 0);
    parse_table_add(&pt, "Log",    PQ_DFL | PQ_NO_EQ_OPT | PQ_BOOL, 0,
                    (void *)&log_flag, 0);

    parse_table_add(&pt, "RedirPort", PQ_DFL | PQ_STRING, 0,
                    (void *)&redir_port_str, NULL);
    parse_table_add(&pt, "MirrorPort", PQ_DFL | PQ_STRING, 0,
                    (void *)&mirror_port_str, NULL);

    /* Matching parameters */
    parse_table_add(&pt, "SrcMac", PQ_DFL | PQ_MAC, 0,
                    (void *)&rule->src_mac, 0);
    parse_table_add(&pt, "DstMac", PQ_DFL | PQ_MAC, 0,
                    (void *)&rule->dst_mac, 0);

    parse_table_add(&pt, "SrcIp", PQ_DFL | PQ_IP, 0,
                    (void *)&rule->src_ip, 0);
    parse_table_add(&pt, "SrcIpMask", PQ_DFL | PQ_IP, 0,
                    (void *)&rule->src_ip_mask, 0);
    parse_table_add(&pt, "DstIp", PQ_DFL | PQ_IP, 0,
                    (void *)&rule->dst_ip, 0);
    parse_table_add(&pt, "DstIpMask", PQ_DFL | PQ_IP, 0,
                    (void *)&rule->dst_ip_mask, 0);
 
    parse_table_add(&pt, "SrcIp6", PQ_DFL | PQ_IP6, 0,
                    (void *)&rule->src_ip6, 0);
    parse_table_add(&pt, "SrcIp6Mask", PQ_DFL | PQ_IP6, 0,
                    (void *)&rule->src_ip6_mask, 0);
    parse_table_add(&pt, "DstIp6", PQ_DFL | PQ_IP6, 0,
                    (void *)&rule->dst_ip6, 0);
    parse_table_add(&pt, "DstIp6Mask", PQ_DFL | PQ_IP6, 0,
                    (void *)&rule->dst_ip6_mask, 0);

    parse_table_add(&pt, "SrcPortMIn", PQ_DFL | PQ_INT, 0,
                    (void *)&rule->src_port_min, 0);
    parse_table_add(&pt, "SrcPortMAx", PQ_DFL | PQ_INT, 0,
                    (void *)&rule->src_port_max, 0);

    parse_table_add(&pt, "DstPortMIn", PQ_DFL | PQ_INT, 0,
                    (void *)&rule->dst_port_min, 0);
    parse_table_add(&pt, "DstPortMAx", PQ_DFL | PQ_INT, 0,
                    (void *)&rule->dst_port_max, 0);

    parse_table_add(&pt, "VlanMIn",    PQ_DFL | PQ_INT, 0,
                    (void *)&vlan_min, 0);
    parse_table_add(&pt, "VlanMAx",    PQ_DFL | PQ_INT, 0,
                    (void *)&vlan_max, 0);

    parse_table_add(&pt, "IpProtocol", PQ_DFL | PQ_MULTI , 0,
                    (void *)&rule->ip_protocol, bcma_acl_ip_protocol_text);
    parse_table_add(&pt, "EtherType", PQ_DFL | PQ_HEX, 0,
                    (void *)&ethertype, 0);
 
    /* Parse redirection port, if specified. */
    if (pt.pt_entries[3].pq_type & PQ_PARSED && redir_port_str != NULL) {
        uport = bcmx_uport_parse(redir_port_str, NULL);
        lport = bcmx_uport_to_lport(uport);
        
        if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: bad redirection port given: %s\n",
                           ARG_CMD(args), redir_port_str);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
        } else {
            rule->actions.flags |= BCMA_ACL_ACTION_REDIR;
            rule->actions.redir_port = lport;
        }
    }

    /* Parse mirror-to-port, if specified. */
    if (pt.pt_entries[4].pq_type & PQ_PARSED && mirror_port_str != NULL) {
        uport = bcmx_uport_parse(mirror_port_str, NULL);
        lport = bcmx_uport_to_lport(uport);
        
        if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: bad mirror port given: %s\n",
                           ARG_CMD(args), mirror_port_str);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
        } else {
            rule->actions.flags |= BCMA_ACL_ACTION_MIRROR;
            rule->actions.mirror_port = lport;
        }
    }

    if(!parseEndOk(args, &pt, &retval)) {
        return retval;
    }
    if (permit_flag) {
       rule->actions.flags |= BCMA_ACL_ACTION_PERMIT;
    }
    if (deny_flag) {
       rule->actions.flags |= BCMA_ACL_ACTION_DENY;
    }
 
    if (log_flag) {
       rule->actions.flags |= BCMA_ACL_ACTION_LOG;
    }


    if (!MAC_IS_CLEAR(rule->src_mac)) {
	rule->flags |= BCMA_ACL_RULE_SRC_MAC;
    }

    if (!MAC_IS_CLEAR(rule->dst_mac)) {
	rule->flags |= BCMA_ACL_RULE_DST_MAC;
    }

    if (rule->src_port_min != 0) {
        rule->flags |= BCMA_ACL_RULE_L4_SRC_PORT;
        if (rule->src_port_max == 0) {
            rule->src_port_max = rule->src_port_min;
        }
    }

    if (rule->src_port_max != 0) {
        rule->flags |= BCMA_ACL_RULE_L4_SRC_PORT;
        if (rule->src_port_min == 0) {
            rule->src_port_min = rule->src_port_max;
        }
    }

    if (rule->dst_port_min != 0) {
        rule->flags |= BCMA_ACL_RULE_L4_DST_PORT;
        if (rule->dst_port_max == 0) {
            rule->dst_port_max = rule->dst_port_min;
        }
    }

    if (rule->dst_port_max != 0) {
        rule->flags |= BCMA_ACL_RULE_L4_DST_PORT;
        if (rule->dst_port_min == 0) {
            rule->dst_port_min = rule->dst_port_max;
        }
    }

    if (rule->src_ip || rule->src_ip_mask) {
       rule->flags |= BCMA_ACL_RULE_SRC_IP4;
    }

    if (rule->dst_ip || rule->dst_ip_mask) {
       rule->flags |= BCMA_ACL_RULE_DST_IP4;
    }

    if (!IP6_IS_CLEAR(rule->src_ip6)) {
       rule->flags |= BCMA_ACL_RULE_SRC_IP6;
    }

    if (!IP6_IS_CLEAR(rule->dst_ip6)) {
       rule->flags |= BCMA_ACL_RULE_DST_IP6;
    }

    if (vlan_min || vlan_max) {
        rule->flags |= BCMA_ACL_RULE_VLAN;
        rule->vlan_min = vlan_min;
	rule->vlan_max = vlan_max;
    }

    if (rule->ip_protocol != BCMA_ACL_IPPROTO_ANY) {
        rule->flags |= BCMA_ACL_RULE_IPPROTOCOL;
    }

    if (ethertype > 0xffff) {
        cli_out("Ethertype=%#x too large (>0xffff).\n", ethertype);
        sal_free(rule);
        return CMD_FAIL;
    }

    if (ethertype != 0) {
        rule->flags |= BCMA_ACL_RULE_ETHERTYPE;
        rule->ether_type = ethertype;
    }

    if (BCM_FAILURE(bcma_acl_rule_add(list_id_cur, rule))) {
        cli_out("ACL: rule add failed (rule_id=%d).\n", rule->rule_id);
        sal_free(rule);
        return CMD_FAIL;
    }

    return CMD_OK;
}

/* 
 * Function:
 *     _bcmx_acl_rule_add
 * Purpose:
 *     
 * Returns:
 *     pointer to new rule or NULL on error
 */
int
_bcmx_acl_rule_add(bcma_acl_list_id_t list_id, bcma_acl_rule_t *rule)
{
    int                 idx1, idx2;

    assert(rule != NULL);

    /* Find the corresponding cmd_acl_list_t */
    for (idx1 = 0; idx1 < BCMA_ACL_NUMB_LISTS; idx1++) {
        if (acl_lists[idx1] == NULL) {
            continue;
	}
        if (acl_lists[idx1]->list.list_id == list_id) {
            /* find an unused rule in list. */
            for (idx2 = 0; idx2 < BCMA_ACL_NUMB_LISTS; idx2++) {
	        if (acl_lists[idx1]->rules[idx2] == NULL) {
                    acl_lists[idx1]->rules[idx2] = rule;
                    return BCM_E_NONE;
		}
	    }
            return BCM_E_RESOURCE;
	}
    }
    return BCM_E_NOT_FOUND;
}


/* 
 * Function:
 *     _bcmx_acl_rule_alloc
 * Purpose:
 *     Allocate a new rule and zero it out.
 * Returns:
 *     pointer to new rule or NULL on error
 */
STATIC bcma_acl_rule_t *
_bcmx_acl_rule_alloc(void) {
    bcma_acl_rule_t     *rule_new;

    LOG_DEBUG(BSL_LS_APPL_ACCESSCTRLLIST,
              (BSL_META("BEGIN _bcmx_acl_rule_alloc()\n")));
    rule_new = sal_alloc(sizeof(bcma_acl_rule_t), "acl rule");
    if (rule_new == NULL) {
        cli_out("ACL Error: allocation failure\n");
        return NULL;
    }
    sal_memset(rule_new, 0, sizeof(bcma_acl_rule_t));

    return rule_new;
}

#endif /* INCLUDE_ACL */
