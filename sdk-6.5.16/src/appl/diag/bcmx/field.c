/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:     field.c
 * Purpose:  BCMX Field commands
 */

#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#include <bcmx/bcmx.h>
#include <bcmx/field.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#ifdef INCLUDE_BCMX

#include "bcmx.h"
#include <appl/diag/diag.h>

#ifdef BCM_FIELD_SUPPORT

char bcmx_cmd_field_usage[] =
    "\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "    field {action | counter | entry | group | list| init | meter | qset} "
    "<args>\n"
#else
    "    field action {add | remove | show} <args> "
    "- Set actions for a field entry\n"
    "    field counter {create | destroy | set |   "
    "- Manage a field counter\n"
    "                   share | show} <args>\n"
    "    field entry {copy | create | destroy |    "
    "- Manage a field entry\n"
    "                 install | qualify |\n"
    "                 reinstall | remove} <args>\n"
    "    field group {create | destroy |           " 
    "- Manage a field group\n"
    "                 set | show} <args>\n"
    "    field list {actions | qualifiers}         "
    "- Display action or qualifier names\n"
    "    field init                                "
    "- Initialize the Field subsystem\n"
    "    field range {create | show | destroy}      "
    "- Manage a TCP/UDP port range checker\n"
    "    field meter {create | destroy | set |     "
    "- Manage a field meter\n"
    "                 share | show} <args>\n"
    "    field qset {create | destroy | set |      "
    "- Manage a field qualifier set\n"
    "                show} <args>\n"
#endif
;

static cmd_result_t action_cmd(int unit, args_t *args);
static cmd_result_t action_add_cmd(int unit, args_t *args);
static cmd_result_t action_remove_cmd(int unit, args_t *args);
static cmd_result_t action_show_cmd(int unit, args_t *args);

static cmd_result_t entry_cmd(int unit, args_t *args);
static cmd_result_t entry_copy_cmd(int unit, args_t *args);
static cmd_result_t entry_create_cmd(int unit, args_t *args);
static cmd_result_t entry_destroy_cmd(int unit, args_t *args);
static cmd_result_t entry_install_cmd(int unit, args_t *args);
static cmd_result_t entry_qualify_cmd(int unit, args_t *args);
static cmd_result_t entry_reinstall_cmd(int unit, args_t *args);
static cmd_result_t entry_remove_cmd(int unit, args_t *args);

static cmd_result_t group_cmd(int unit, args_t *args);
static cmd_result_t group_create_cmd(int unit, args_t *args);
static cmd_result_t group_destroy_cmd(int unit, args_t *args);
static cmd_result_t group_set_cmd(int unit, args_t *args);
static cmd_result_t group_show_cmd(int unit, args_t *args);

static cmd_result_t init_cmd(int unit, args_t *args);

static cmd_result_t list_cmd(int unit, args_t *args);
static cmd_result_t list_actions_cmd(int unit, args_t *args);
static cmd_result_t list_quals_cmd(int unit, args_t *args);

static cmd_result_t range_cmd(int unit, args_t *args);

static cmd_result_t qset_cmd(int unit, args_t *args);
static cmd_result_t qset_create_cmd(int unit, args_t *args);
static cmd_result_t qset_destroy_cmd(int unit, args_t *args);
static cmd_result_t qset_set_cmd(int unit, args_t *args);
static cmd_result_t qset_show_cmd(int unit, args_t *args);

static cmd_t field_cmd_list[] = {
    {"Action", action_cmd,
     "field action {add | remove | show} <args>",
     "Set actions for a field processor entry."},
    {"Entry", entry_cmd,
     "field entry {copy | create | destroy | install | qualify |\n"
     "                  reinstall | remove} <args>", 
     "Manage a field processor entry."},

    {"Group", group_cmd,
     "field group {create | destroy | set | show} <args>",
     "Manage a field processor group."},

    {"Init", init_cmd, 
     "field init",
     "Initialize the Field Processor subsystem."},

    {"List", list_cmd, 
     "field list {actions | qualifiers}",
     "List action or qualfier names"},

    {"Range", range_cmd,
     "field range {create <flags> [Min= ] [Max= ] | show <range-id> | destroy <range-id>} ",
     "Create, destroy or display range checks."},
    {"QSET", qset_cmd,
     "field qset {create | destroy | set | show} <args>",
     "Manage a field qualifier set."}
};

static cmd_t field_action_cmd_list[] = {
    {"Add", action_add_cmd, 
     "field action add <entry-id> Action=<action-name> Param0=<value> "
     "Param1=<value>", 
     "Add an action for the specified field entry."},

    {"Remove", action_remove_cmd, 
     "field action remove <entry-id> Action=<action-name>", 
     "Remove the action(s) from the specified field entry."},

    {"SHOW", action_show_cmd, 
     "field action show <entry-id> Action=<action-name>",
     "Display the parameter values of the specified action for a field entry."}
};

static cmd_t field_entry_cmd_list[] = {
    {"CoPy", entry_copy_cmd, 
     "field entry copy <entry-id>", 
     "Create a copy of an existing field entry."},

    {"Create", entry_create_cmd, 
     "field entry create <group-id>", 
     "Create an empty field entry based on a field group."},

    {"Destroy", entry_destroy_cmd, 
     "field entry destroy {<entry-id> | ALL}", 
     "Destroy a field entry or all existing field entries."},

    {"Install", entry_install_cmd, 
     "field entry install <entry-id>", 
     "Install a field entry into the hardware tables."},

    {"Qualify", entry_qualify_cmd,
     "field entry qualify <entry-id> FieldQualifier=<qualifier-name>\n"
     "                                    <data> [<mask>]",
     "Set data and mask for the specified field qualifier."},

    {"ReInstall", entry_reinstall_cmd, 
     "field entry reinstall <entry-id>", 
     "Re-install a field entry into the hardware tables."},

    {"ReMove", entry_remove_cmd,
     "field entry remove <entry-id>",
     "Remove a field entry from the hardware tables."}
};

static cmd_t field_group_cmd_list[] = {
    {"Create", group_create_cmd, 
     "field group create QSET=<qset-number> PRI=<number>", 
     "Create a field group with specified priority and qualifier set."},

    {"Destroy", group_destroy_cmd, 
     "field group destroy <group-id>", 
     "Deallocate a field group.  All entries for this\n"
     "        group must have been previously destroyed."},

    {"Set", group_set_cmd, 
     "field group set <group-id> QSET=<qset-number>", 
     "Set the qualifier set for a field group."},

    {"SHOW", group_show_cmd, 
     "field group show <group-id>", 
     "Display the qualifier set for a field group."}
};

static cmd_t field_list_cmd_list[] = {
    {"ACTions", list_actions_cmd, 
     "field list actions",
     "Display Field actions."},

    {"QUALifiers", list_quals_cmd, 
     "field list qualifiers", 
     "Display Field qualifiers."},
};

static cmd_t field_qset_cmd_list[] = {
    {"Create", qset_create_cmd, 
     "field qset create", 
     "Create a field qualifier set.  No field qualifier will be selected\n"
     "        for the new set."},

    {"Destroy", qset_destroy_cmd, 
     "field qset destroy <qset-number>", 
     "Destroys a field qualifier set."},

    {"Set", qset_set_cmd, 
     "field qset set <qset-number> {<qualifier-name>=<on|off> ...}", 
     "Set or clear field qualifiers for the specified field qualifier set.\n"
     "        One or more qualifiers can be specified.  Qualifiers not\n"
     "        specified are left unchanged."},

    {"SHOW", qset_show_cmd, 
     "field qset show <qset-number>", 
     "Display a field qualifier set attributes."}
};


/* Static Globals and Defines */

/* FP Qualifiers */
#define FP_QUALIFIER_CNT  bcmFieldQualifyCount 
static char *fp_qualifier_names[FP_QUALIFIER_CNT+1] = BCM_FIELD_QUALIFY_STRINGS;
#define FP_QUALIFIER_NAME_GET(qid)    \
    ((((qid) < 0) || ((qid) >= FP_QUALIFIER_CNT)) ? \
     NULL : fp_qualifier_names[qid])

/* FP Actions */
#define FP_ACTION_CNT     bcmFieldActionCount
static char *fp_action_names[FP_ACTION_CNT+1] = BCM_FIELD_ACTION_STRINGS;
#define FP_ACTION_NAME_GET(action)    \
    ((((action) < 0) || ((action) >= FP_ACTION_CNT)) ? \
     NULL : fp_action_names[action])

/* 
 * Table:   
 *     Array of SETS of field qualifier.
 * Element:  
 *     A set of field qualifiers contains the selection of qualifiers
 *     for the the specified set, and a flag indicating if the set was
 *     created.
 */
typedef struct fp_qset_s {
    bcm_field_qset_t    qset;
    int                 created;    /* Qset is used and create */
} fp_qset_t;

#define MAX_QSETS    100
static  fp_qset_t    fp_qset_tbl[MAX_QSETS];


/* 
 * Function:    
 *     get_integer
 * Purpose:    
 *     Gets an integer value from a string, and, if specified, 
 *     checks that value is within allowed range.
 * Parameters:  
 *     cmd         - Command name string
 *     subcmd_str  - Subcommand name string
 *     param       - String that contains number to be parsed
 *     number      - Returns number parsed
 *     number_str  - Name of number (to use when displaying error message)
 *     check_range - Indicates whether to check value within range
 *     min         - Minimum value allowed
 *     max         - Maximum value allowed
 * Returns:     
 *     CMD_OK      - Success, valid integer
 *     CMD_FAIL    - Failure, could not parse integer from string,
 *                   or value is out of range.
 */
static cmd_result_t
get_integer(char *cmd, char *subcmd_str, 
            char *param, int *number, char *number_str,
            int check_range, int min, int max)
{
    if (param == NULL) {
        sal_printf("%s: Need %s\n", cmd, number_str);
        return CMD_FAIL;
    }

    if (!isint(param)) {
        sal_printf("%s: Invalid %s '%s'\n", cmd, number_str, param);
        return CMD_FAIL;
    }
    *number = parse_integer(param);

    if (check_range) {
        if ((*number < min) || (*number > max)) {
            sal_printf("%s: %s failed: %s must be %d..%d\n",
                       cmd, subcmd_str, number_str, min, max);
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}


static void
display_qset(bcm_field_qset_t qset)
{
    bcm_field_qualify_t  qid;
    char                 *qname_ptr;
    char                 *set = "X";
    char                 *clear = "-";
    char                 *str;
    int                  num_qualifiers_row = 3;

    sal_printf("     Qualifier settings   ('X'= set    '-'= clear)\n");
    sal_printf("    +---------------------+---------------------+"
               "---------------------+");

    for (qid = (bcm_field_qualify_t)0; qid < FP_QUALIFIER_CNT; qid++) {
        if ((qname_ptr = FP_QUALIFIER_NAME_GET(qid)) == NULL) {
            sal_printf("Could not find qualifier=%d\n", qid);
            continue;
        }

        if ((qid % num_qualifiers_row) == 0) {
            sal_printf("\n    |");
        }
        if (BCM_FIELD_QSET_TEST(qset, qid)) {
            str = set;
        } else {
            str = clear;
        }
        sal_printf(" %-18s%1s |", qname_ptr, str);
    }

    while ((qid++ % num_qualifiers_row) != 0) {
        sal_printf("%21s|", " ");
    }
    sal_printf("\n    +---------------------+---------------------+"
               "---------------------+\n\n");

    return;
}

cmd_result_t
bcmx_cmd_field(int unit, args_t *args)
{
    static int  first_time = TRUE;

    if (first_time) {
        memset(fp_qset_tbl, 0, sizeof(fp_qset_tbl));
        fp_qualifier_names[FP_QUALIFIER_CNT] = NULL;
        fp_action_names[FP_ACTION_CNT] = NULL;
        first_time = FALSE;
    }

    return subcommand_execute(unit, args, 
                              field_cmd_list, COUNTOF(field_cmd_list));
}

static cmd_result_t
init_cmd(int unit, args_t *args)
{
    int    rv;
    char   *cmd;

    cmd = ARG_CMD(args);

    if (ARG_CNT(args)) {
        sal_printf("%s: Invalid option %s\n", cmd, ARG_CUR(args));
        return CMD_FAIL;
    }

    rv = bcmx_field_init();

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Subsystem initialization failed: %s\n", cmd,
                   bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
list_cmd(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              field_list_cmd_list, 
                              COUNTOF(field_list_cmd_list));
}

static cmd_result_t
list_actions_cmd(int unit, args_t *args)
{
    bcm_field_action_t  action;
    char                buf[BCM_FIELD_ACTION_WIDTH_MAX];

    for (action = (bcm_field_action_t)0; action < bcmFieldActionCount; action++) {
        cli_out("\t%s\n", format_field_action(buf, action, 0));
    }
    return CMD_OK;
}

static cmd_result_t
list_quals_cmd(int unit, args_t *args)
{
    bcm_field_qualify_t qual;
    char                buf[BCM_FIELD_QUALIFY_WIDTH_MAX];

    for (qual = (bcm_field_qualify_t)0; qual < bcmFieldQualifyCount; qual++) {
        cli_out("\t%s\n", format_field_qualifier(buf, qual, 0));
    }
    return CMD_OK;
}


static cmd_result_t
action_cmd(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              field_action_cmd_list, 
                              COUNTOF(field_action_cmd_list));
}

static cmd_result_t
action_add_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    cmd_result_t         ret_code;
    bcm_field_entry_t    entry;
    uint32               param0 = 0, param1 = 0;
    int                  action = -1;
    bcm_field_action_t   action_id;
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Action add", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "Action",  PQ_DFL | PQ_MULTI, 
                    0, &action,  fp_action_names);
    parse_table_add(&pt, "Param0",  PQ_DFL | PQ_INT,
                    0, &param0, NULL);
    parse_table_add(&pt, "Param1",  PQ_DFL | PQ_INT,
                    0, &param1, NULL);
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    if ((action < 0) || (action >= FP_ACTION_CNT)) {
        sal_printf("%s: Invalid Field Action selection (%d)\n", cmd, action);
        return CMD_FAIL;
    }

    action_id = (bcm_field_action_t)action;

    rv = bcmx_field_action_add(entry, action_id, param0, param1);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Action add failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
action_remove_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    char                 *param;
    bcm_field_entry_t    entry;
    int                  action = -1;
    parse_table_t        pt;


    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Action remove", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    if (!ARG_CNT(args)) {
        sal_printf("%s: Need option\n", cmd);
        return CMD_FAIL;
    }

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "Action",  PQ_DFL | PQ_MULTI,
                    0, &action,  fp_action_names);
    if ((parse_arg_eq(args, &pt) < 0)) {
        sal_printf("%s: Invalid option %s\n", cmd, ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);   /* No longer needed */

    if (action == -1) {    /* Check if 'ALL' was specified */
        param = ARG_GET(args);
        if (sal_strcasecmp(param, "all")) {
            sal_printf("%s: Invalid option %s\n", cmd, param);
            return CMD_FAIL;
        }
        rv = bcmx_field_action_remove_all(entry);
    } else {
        if ((action < 0) || (action >= FP_ACTION_CNT)) {
            sal_printf("%s: Invalid Field Action selection (%d)\n", 
                       cmd, action);
            return CMD_FAIL;
        }
        rv = bcmx_field_action_remove(entry, (bcm_field_action_t)action);
    }

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Action remove failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
action_show_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    cmd_result_t         ret_code;
    bcm_field_entry_t    entry;
    uint32               param0 = 0, param1 = 0;
    int                  action = -1;
    bcm_field_action_t   action_id;
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Action show",
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "Action",  PQ_DFL | PQ_MULTI,
                    0, &action,  fp_action_names);
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    if ((action < 0) || (action >= FP_ACTION_CNT)) {
        sal_printf("%s: Invalid Field Action selection (%d)\n", cmd, action);
        return CMD_FAIL;
    }

    action_id = (bcm_field_action_t)action;

    rv = bcmx_field_action_get(entry, action_id, &param0, &param1);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Action show failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    sal_printf("%s: Parameter values for Entry=%d(0x%x), Action=%s are:\n"
               "                 Param0=%u\n"
               "                 Param1=%u\n",
               cmd, entry, entry, FP_ACTION_NAME_GET(action),
               param0, param1);

    return CMD_OK;
}

static cmd_result_t
entry_cmd(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              field_entry_cmd_list, 
                              COUNTOF(field_entry_cmd_list));
}

static cmd_result_t
entry_create_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_group_t    group;
    bcm_field_entry_t    entry = 0;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry create", 
                    ARG_GET(args), &group, "group id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_entry_create(group, &entry);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry create failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    sal_printf("%s: Entry=%d(0x%x) created for group=%d(0x%x)\n",
               cmd, entry, entry, group, group);

    return CMD_OK;
}

static cmd_result_t
entry_destroy_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    char                 *param;
    bcm_field_entry_t    entry;

    cmd = ARG_CMD(args);

    if ((param = ARG_GET(args)) == NULL) {
        sal_printf("%s: Entry destroy failed: Entry id needed\n", cmd);
        return CMD_FAIL;
    }

    if (sal_strcasecmp(param, "all") == 0) {
        if (ARG_CNT(args)) {
            sal_printf("%s: Invalid option %s\n", cmd, ARG_CUR(args));
            return CMD_FAIL;
        }
        rv = bcmx_field_entry_destroy_all();
    } else {
        if (get_integer(cmd, "Entry destroy", 
                        param, &entry, "entry id",
                        FALSE, 0, 0) != CMD_OK) {
            return CMD_FAIL;
        }

        rv = bcmx_field_entry_destroy(entry);
    }

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry destroy failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
entry_copy_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_entry_t    src_entry, dst_entry = 0;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry copy", 
                    ARG_GET(args), &src_entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_entry_copy(src_entry, &dst_entry);    

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry copy failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    sal_printf("%s: src-entry=%d(0x%x) copied to dst-entry=%d(0x%x)\n",
               cmd, src_entry, src_entry, dst_entry, dst_entry);

    return CMD_OK;
}

static cmd_result_t
entry_install_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_entry_t    entry;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry install", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_entry_install(entry);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry install failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
entry_qualify_cmd(int unit, args_t *args)
{
    int                  rv = BCM_E_NONE;
    char                 *cmd;
    char                 *param;
    bcm_field_entry_t    entry;
    int                  qualifier = -1;
    bcm_field_qualify_t  qid;
    int                  data, mask;
    bcmx_lplist_t        lplist;
    bcm_trunk_t          trunk;
    bcmx_lport_t         lport;
    bcm_ip_t             ipaddr, ipaddr_mask;
    bcm_ip6_t            ip6addr, ip6addr_mask;
    bcm_mac_t            macaddr, macaddr_mask;
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry qualify", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "FieldQualifier",  PQ_DFL | PQ_MULTI,
                    0, &qualifier, fp_qualifier_names);
    if ((parse_arg_eq(args, &pt) < 0)) {
        sal_printf("%s: Invalid option %s\n", cmd, ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);   /* No longer needed */

    if ((qualifier < 0) || (qualifier >= FP_QUALIFIER_CNT)) {
        sal_printf("%s: Invalid Field Qualifier selection (%d)\n",
                   cmd, qualifier);
        return CMD_FAIL;
    }

    qid = (bcm_field_qualify_t)qualifier;

    switch (qid) {
    case bcmFieldQualifyInPorts:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: Need user port list\n", cmd);
            return CMD_FAIL;
        }
        bcmx_lplist_init(&lplist, 0, 0);
        if (bcmx_lplist_parse(&lplist, param) < 0) {
            sal_printf("%s: Invalid user port list %s\n", cmd, param);
            return CMD_FAIL;
        }
        rv = bcmx_field_qualify_InPorts(entry, lplist);
        break;

    case bcmFieldQualifySrcPort:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: Need source user port\n", cmd);
            return CMD_FAIL;
        }
        lport = bcmx_uport_to_lport(bcmx_uport_parse(param, NULL));
        if (lport == BCMX_NO_SUCH_LPORT) {
            sal_printf("%s: Invalid source user port %s\n", cmd,
                       bcmx_lport_to_uport_str(lport));
            return CMD_FAIL;
        }
        rv = bcmx_field_qualify_SrcPort(entry, lport);
        break;

    case bcmFieldQualifySrcTrunk:
        if (get_integer(cmd, "Entry qualify", 
                        ARG_GET(args), &trunk, "source trunk id",
                        FALSE, 0, 0) != CMD_OK) {
            return CMD_FAIL;
        }
        rv = bcmx_field_qualify_SrcTrunk(entry, trunk);
        break;

    case bcmFieldQualifyDstPort:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: Need destination user port\n", cmd);
            return CMD_FAIL;
        }
        lport = bcmx_uport_to_lport(bcmx_uport_parse(param, NULL));
        if (lport == BCMX_NO_SUCH_LPORT) {
            sal_printf("%s: Invalid destination user port %s\n", cmd,
                       bcmx_lport_to_uport_str(lport));
            return CMD_FAIL;
        }
        rv = bcmx_field_qualify_DstPort(entry, lport);
        break;

    case bcmFieldQualifyDstTrunk:
        if (get_integer(cmd, "Entry qualify", 
                        ARG_GET(args), &trunk, "destination trunk id",
                        FALSE, 0, 0) != CMD_OK) {
            return CMD_FAIL;
        }
        rv = bcmx_field_qualify_DstTrunk(entry, trunk);
        break;

    case bcmFieldQualifyEtherType:
    case bcmFieldQualifyIpProtocol:
    case bcmFieldQualifyIpInfo:
    case bcmFieldQualifyPacketRes:
    case bcmFieldQualifyDSCP:
    case bcmFieldQualifyIpFlags:
    case bcmFieldQualifyTcpControl:
    case bcmFieldQualifyTtl:
    case bcmFieldQualifyIp6FlowLabel:
    case bcmFieldQualifyMHOpcode:
    case bcmFieldQualifyL4SrcPort:
    case bcmFieldQualifyL4DstPort:
    case bcmFieldQualifyRangeCheck:
    case bcmFieldQualifyOuterVlan:
    case bcmFieldQualifyInnerVlan:


        if (get_integer(cmd, "Entry qualify", 
                        ARG_GET(args), &data, "data",
                        FALSE, 0, 0) != CMD_OK) {
            return CMD_FAIL;
        }
        if (get_integer(cmd, "Entry qualify",
                        ARG_GET(args), &mask, "mask",
                        FALSE, 0, 0) != CMD_OK) {
            return CMD_FAIL;
        }

        if (qid == bcmFieldQualifyEtherType) {
            rv = bcmx_field_qualify_EtherType(entry, data, mask);
        } else if (qid == bcmFieldQualifyIpProtocol) {
            rv = bcmx_field_qualify_IpProtocol(entry, data, mask);
        } else if (qid == bcmFieldQualifyIpInfo) {
            rv = bcmx_field_qualify_IpInfo(entry, data, mask);   
        } else if (qid == bcmFieldQualifyPacketRes) {
            rv = bcmx_field_qualify_PacketRes(entry, data, mask); 
        } else if (qid == bcmFieldQualifyDSCP) {
            rv = bcmx_field_qualify_DSCP(entry, data, mask);
        } else if (qid == bcmFieldQualifyIpFlags) {
            rv = bcmx_field_qualify_IpFlags(entry, data, mask);
        } else if (qid == bcmFieldQualifyTcpControl) {
            rv = bcmx_field_qualify_TcpControl(entry, data, mask);
        } else if (qid == bcmFieldQualifyTtl) {
            rv = bcmx_field_qualify_Ttl(entry, data, mask);
        } else if (qid == bcmFieldQualifyIp6FlowLabel) {
            rv = bcmx_field_qualify_Ip6FlowLabel(entry, data, mask);
        } else if (qid == bcmFieldQualifyMHOpcode) {
            rv = bcmx_field_qualify_MHOpcode(entry, data, mask);
        } else if (qid == bcmFieldQualifyL4SrcPort) {
            rv = bcmx_field_qualify_L4SrcPort(entry, data, mask);
        } else if (qid == bcmFieldQualifyL4DstPort) {
            rv = bcmx_field_qualify_L4DstPort(entry, data, mask);
        } else if (qid == bcmFieldQualifyRangeCheck) {
            rv = bcmx_field_qualify_RangeCheck(entry, data, mask);
        } else if (qid == bcmFieldQualifyOuterVlan) {
            rv = bcmx_field_qualify_OuterVlan(entry, data, mask);
        } else if (qid == bcmFieldQualifyInnerVlan) {
            rv = bcmx_field_qualify_InnerVlan(entry, data, mask);
        }

        break;

    case bcmFieldQualifySrcIp:
    case bcmFieldQualifyDstIp:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: IP address required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_ipaddr(param, &ipaddr)) {
            sal_printf("%s: Invalid IP address %s\n", cmd, param);
            return CMD_FAIL;
        }

        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: IP address mask required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_ipaddr(param, &ipaddr_mask)) {
            sal_printf("%s: Invalid IP address mask %s\n", cmd, param);
            return CMD_FAIL;
        }

        if (qid == bcmFieldQualifySrcIp) {
            rv = bcmx_field_qualify_SrcIp(entry, ipaddr, ipaddr_mask);
        } else if (qid == bcmFieldQualifyDstIp) {
            rv = bcmx_field_qualify_DstIp(entry, ipaddr, ipaddr_mask);
        }

        break;

    case bcmFieldQualifySrcIp6:
    case bcmFieldQualifyDstIp6:
    case bcmFieldQualifyDstIp6High:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: IPv6 address required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_ip6addr(param, ip6addr)) {
            sal_printf("%s: Invalid IPv6 address %s\n", cmd, param);
            return CMD_FAIL;
        }

        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: IPv6 address mask required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_ip6addr(param, ip6addr_mask)) {
            sal_printf("%s: Invalid IPv6 address mask %s\n", cmd, param);
            return CMD_FAIL;
        }

        if (qid == bcmFieldQualifySrcIp6) {
            rv = bcmx_field_qualify_SrcIp6(entry, ip6addr, ip6addr_mask);
        } else if (qid == bcmFieldQualifyDstIp6) {
            rv = bcmx_field_qualify_DstIp6(entry, ip6addr, ip6addr_mask);
        } else if (qid == bcmFieldQualifyDstIp6High) {
            rv = bcmx_field_qualify_DstIp6High(entry, ip6addr, ip6addr_mask);
        }

        break;


    case bcmFieldQualifySrcMac:
    case bcmFieldQualifyDstMac:
        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: MAC address required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_macaddr(param, macaddr)) {
            sal_printf("%s: Invalid MAC address %s\n", cmd, param);
            return CMD_FAIL;
        }

        if ((param = ARG_GET(args)) == NULL) {
            sal_printf("%s: MAC address mask required\n", cmd);
            return CMD_FAIL;
        }
        if (parse_macaddr(param, macaddr_mask)) {
            sal_printf("%s: Invalid MAC address mask %s\n", cmd, param);
            return CMD_FAIL;
        }

        if (qid == bcmFieldQualifySrcMac) {
            rv = bcmx_field_qualify_SrcMac(entry, macaddr, macaddr_mask);
        } else if (qid == bcmFieldQualifyDstMac) {
            rv = bcmx_field_qualify_DstMac(entry, macaddr, macaddr_mask);
        }

        break;

    default:
        sal_printf("%s: Invalid Field Qualifier selection\n", cmd);
        return CMD_FAIL;

    }
        
    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry qualify failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
entry_reinstall_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_entry_t    entry;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry re-install", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_entry_reinstall(entry);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry re-install failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
entry_remove_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_entry_t    entry;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Entry remove", 
                    ARG_GET(args), &entry, "entry id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_entry_remove(entry);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Entry remove failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
group_cmd(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              field_group_cmd_list, 
                              COUNTOF(field_group_cmd_list));

}

static cmd_result_t
group_create_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    cmd_result_t         ret_code;
    bcm_field_group_t    group = 0;
    int                  qset_num = -1;
    int                  pri = 0;
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "QSET", PQ_DFL | PQ_INT, 0, &qset_num,  NULL);
    parse_table_add(&pt, "PRI",  PQ_DFL | PQ_INT, 0, &pri, NULL);
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    if ((qset_num < 0) || (qset_num >= MAX_QSETS)) {
        sal_printf("%s: Group create failed: Qualifier set number must be "
                   "0..%d\n", cmd, (MAX_QSETS - 1));
        return CMD_FAIL;
    }

    if (!fp_qset_tbl[qset_num].created) {
        sal_printf("%s: Group create failed: qset=%d has not been "
                   "created\n", cmd, qset_num);
        return CMD_FAIL;
    }

    rv = bcmx_field_group_create(fp_qset_tbl[qset_num].qset, pri, &group);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Group create failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    sal_printf("%s: Group=%d(0x%x) created\n", cmd, group, group);

    return CMD_OK;
}

static cmd_result_t
group_destroy_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_group_t    group;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Group destroy", 
                    ARG_GET(args), &group, "group id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_group_destroy(group);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Group destroy failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
group_set_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    cmd_result_t         ret_code;
    bcm_field_group_t    group;
    int                  qset_num = -1;
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Group qualifier set", 
                    ARG_GET(args), &group, "group id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "QSET", PQ_DFL | PQ_INT, 0, &qset_num,  NULL);
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    if ((qset_num < 0) || (qset_num >= MAX_QSETS)) {
        sal_printf("%s: Group qualifier set failed: Qualifier set number "
                   "must be 0..%d\n", cmd, (MAX_QSETS - 1));
        return CMD_FAIL;
    }

    if (!fp_qset_tbl[qset_num].created) {
        sal_printf("%s: Group qualifier set failed: qset=%d "
                   "has not been created\n", cmd, qset_num);
        return CMD_FAIL;
    }

    rv = bcmx_field_group_set(group, fp_qset_tbl[qset_num].qset);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Group qualifier set failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

typedef enum {
    ra_create, ra_show, ra_destroy
} range_action_t;

static cmd_result_t
range_cmd(int unit, args_t *args)
{
    int                  rv = BCM_E_INTERNAL;
    char                 *cmd;
    cmd_result_t         ret_code;
    char                 *param;
    range_action_t       action;
    int                  range = -1;
    uint32               flags = 0;
    bcm_l4_port_t        min = 0, max = 0;
    parse_table_t        pt;
    bcm_field_range_t    range_id;

    cmd = ARG_CMD(args);

    if ((param = ARG_GET(args)) == NULL) {
        sal_printf("%s: Range subcommand needed\n", cmd);
        return CMD_FAIL;
    }

    if (sal_strcasecmp(param, "create") == 0) {
        action = ra_create;
    } else if (sal_strcasecmp(param, "show") == 0) {
        action = ra_show;
    } else if (sal_strcasecmp(param, "destroy") == 0) {
        action = ra_destroy;
    } else {
        sal_printf("%s: Invalid option '%s'\n", cmd, param);
        return CMD_FAIL;
    }


   /* Parse option */
    parse_table_init(0, &pt);
    parse_table_add(&pt, "Range", PQ_DFL | PQ_INT, 0, &range,  NULL);
    parse_table_add(&pt, "Flags", PQ_DFL | PQ_INT, 0, &flags,  NULL);
    parse_table_add(&pt, "MIN", PQ_DFL | PQ_INT, 0, &min,  NULL);
    parse_table_add(&pt, "MAX", PQ_DFL | PQ_INT, 0, &max,  NULL);
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    if (ARG_CNT(args) != 0) {
        sal_printf("%s: Ignoring extra arguments beginning with %s\n",
                   cmd, ARG_GET(args));
    }

    if ((range < 0) && (action != ra_create)) {
        sal_printf("%s: Range=<value> argument required\n", cmd);
        return CMD_FAIL;
    }
    range_id = range;

    switch (action) {
    case ra_create:
        rv = bcmx_field_range_create(&range_id,
                                     flags, min, max);
        if (BCM_SUCCESS(rv)) {
            sal_printf("%s: Created Range=0x%x, Flags=0x%x, Min=%d, Max=%d\n",
                       cmd, range_id, flags, min, max);
        }
        break;
    case ra_destroy:
        rv = bcmx_field_range_destroy(range_id);
        break;
    case ra_show:
        rv = bcmx_field_range_get(range_id,
                                  &flags, &min, &max);
        if (BCM_SUCCESS(rv)) {
            sal_printf("%s: Range=0x%x, Flags=0x%x, Min=%d, Max=%d\n",
                       cmd, range_id, flags, min, max);
        }
        break;
    }

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Range failed: %s\n", cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
group_show_cmd(int unit, args_t *args)
{
    int                  rv;
    char                 *cmd;
    bcm_field_group_t    group;
    bcm_field_qset_t     qset;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Group qualifier show", 
                    ARG_GET(args), &group, "group id",
                    FALSE, 0, 0) != CMD_OK) {
        return CMD_FAIL;
    }

    rv = bcmx_field_group_get(group, &qset);

    if (BCM_FAILURE(rv)) {
        sal_printf("%s: Group qualifier show failed: %s\n", 
                   cmd, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    sal_printf("     GROUP = %d(0x%x)\n", group, group);
    display_qset(qset);

    return CMD_OK;
}

static cmd_result_t
qset_cmd(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              field_qset_cmd_list, 
                              COUNTOF(field_qset_cmd_list));
}

static cmd_result_t
qset_create_cmd(int unit, args_t *args)
{
    char                 *cmd;
    int                  qset_num = 0;

    cmd = ARG_CMD(args);

    /* Find an available qset index */
    while ((fp_qset_tbl[qset_num].created)) {
        if (++qset_num >= MAX_QSETS) {
            sal_printf("%s: Qset create failed: Reached maximum number of "
                       "qsets\n", cmd);
            return CMD_FAIL;
        }
    }

    fp_qset_tbl[qset_num].created = TRUE;
    BCM_FIELD_QSET_INIT(fp_qset_tbl[qset_num].qset);

    sal_printf("%s: Qset=%d created\n", cmd, qset_num);

    return CMD_OK;
}

static cmd_result_t
qset_destroy_cmd(int unit, args_t *args)
{
    int                  qset_num;

    if (get_integer(ARG_CMD(args), "Qset destroy", 
                    ARG_GET(args), &qset_num, "qset number",
                    TRUE, 0, (MAX_QSETS - 1)) != CMD_OK) {
        return CMD_FAIL;
    }

    fp_qset_tbl[qset_num].created = FALSE;

    return CMD_OK;
}

static cmd_result_t
qset_set_cmd(int unit, args_t *args)
{
    char                 *cmd;
    cmd_result_t         ret_code;
    int                  qset_num, i;
    fp_qset_t            *qset_ptr;
    int                  qualifier[FP_QUALIFIER_CNT];
    parse_table_t        pt;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Qset set",
                    ARG_GET(args), &qset_num, "qset number",
                    TRUE, 0, (MAX_QSETS - 1)) != CMD_OK) {
        return CMD_FAIL;
    }

    if (!fp_qset_tbl[qset_num].created) {
        sal_printf("%s: Qset set failed: qset=%d has not been created\n",
                   cmd, qset_num);
        return CMD_FAIL;
    }

    qset_ptr = &fp_qset_tbl[qset_num];

    /* Parse option */
    parse_table_init(0, &pt);
    for (i = 0; i < FP_QUALIFIER_CNT; i++) {
        qualifier[i] = -1;     /* (-1) will indicate to leave unchanged */
        parse_table_add(&pt, FP_QUALIFIER_NAME_GET(i), PQ_DFL | PQ_BOOL,
                        0, &qualifier[i], NULL);
    }
    if (!parseEndOk(args, &pt, &ret_code)) {
        return ret_code;
    }

    
    for (i = 0; i < FP_QUALIFIER_CNT; i++) {
        /* If qualifier was not specified in command, leave it unchanged */
        if (qualifier[i] == -1) {
            continue;
        }
        if (qualifier[i] == 0) {
            BCM_FIELD_QSET_REMOVE(qset_ptr->qset, i);
        } else {
            BCM_FIELD_QSET_ADD(qset_ptr->qset, i);
        }
    }

    return CMD_OK;
}

static cmd_result_t
qset_show_cmd(int unit, args_t *args)
{
    char                 *cmd;
    int                  qset_num;

    cmd = ARG_CMD(args);

    if (get_integer(cmd, "Qset show", 
                    ARG_GET(args), &qset_num, "qset number",
                    TRUE, 0, (MAX_QSETS - 1)) != CMD_OK) {
        return CMD_FAIL;
    }

    if (!fp_qset_tbl[qset_num].created) {
        sal_printf("%s: Qset show failed: qset=%d has not been created\n",
                   cmd, qset_num);
        return CMD_FAIL;
    }

    sal_printf("     QSET = %d\n", qset_num);
    display_qset(fp_qset_tbl[qset_num].qset);

    return CMD_OK;
}

#endif  /* BCM_FIELD_SUPPORT */
#endif  /* INCLUDE_BCMX */
