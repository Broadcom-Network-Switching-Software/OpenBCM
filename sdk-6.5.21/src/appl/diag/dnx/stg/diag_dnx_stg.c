/** \file diag_dnx_stg.c
 *
 * DNX STG DIAG PACK - diagnostic pack for module stg
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_STGDNX

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/** bcm */
#include <bcm/stg.h>
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnx_stg.h"

/*
 * }
 */

/*
 * Define the short strings for each stp state.
 * Refer to \bcm_stg_stp_e for more information.
 */
/* *INDENT-OFF* }*/
static sh_sand_enum_t sand_diag_stp_state_enum[] = {
    /**String    Value*/
    {"disable",  BCM_STG_STP_DISABLE, "Disable"},
    {"block",    BCM_STG_STP_BLOCK, "Accepts BPDU, does not learn"},
    {"listen",   BCM_STG_STP_LISTEN, "Accepts BPDU, does not learn"},
    {"learn",    BCM_STG_STP_LEARN, "Accepts BPDU and learns, no forwarding"},
    {"forward",  BCM_STG_STP_FORWARD, "Accepts BPDU, normal switching operation"},
    {NULL}
};
/* *INDENT-ON* }*/

/*
 * cmd implementation begin {
 */
/**
 * \brief
 *  Clear the STG model.
 *  CMD format: stg clear
 *
 * \see
 *  bcm_stg_clear()
 */
static shr_error_e
cmd_dnx_stg_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_stg_clear(unit));
    LOG_CLI((BSL_META("STG model has bee cleared successfully!\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Create a STG. If given STG-ID is 0, allocate a STG ID internally,
 *  else create a STG with the given STG ID.
 *  CMD format: stg CReate [stgid=<stg-id>]
 *
 * \see
 *  bcm_stg_create()
 *  bcm_stg_create_id()
 */
static shr_error_e
cmd_dnx_stg_create(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    if (stg_id == 0)
    {
        SHR_IF_ERR_EXIT(bcm_stg_create(unit, &stg_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_stg_create_id(unit, stg_id));
    }

    LOG_CLI((BSL_META("Create the Spaning Tree Group ID: %d\n"), stg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroy a STG.
 *  CMD format: stg destroy stgid=<stg-id>
 *
 * \see
 *  bcm_stg_destroy()
 */
static shr_error_e
cmd_dnx_stg_destroy(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    SHR_IF_ERR_EXIT(bcm_stg_destroy(unit, stg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the stp state for a port.
 *  CMD format: stg stp set id=<stg-id> port=<eth-ports> [state=<stp-state>]
 *
 * \see
 *  bcm_stg_stp_set()
 */
static shr_error_e
cmd_dnx_stg_stp_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;
    bcm_pbmp_t logical_ports;
    bcm_port_t port, logical_port = -1;
    int stp_state, nof_ports = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_GET_ENUM("state", stp_state);

    BCM_PBMP_COUNT(logical_ports, nof_ports);
    if (nof_ports == 0)
    {
        LOG_CLI((BSL_META("Option port with valid port-ids is needed!\n")));
        SHR_EXIT();
    }

    BCM_PBMP_ITER(logical_ports, port)
    {
        logical_port = port;
        SHR_IF_ERR_EXIT(bcm_stg_stp_set(unit, stg_id, logical_port, stp_state));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the stp state for a port.
 *  CMD format: stg stp get [id=<stg-id>] port=<eth-ports>
 *
 * \see
 *  bcm_stg_stp_get()
 */
static shr_error_e
cmd_dnx_stg_stp_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;
    bcm_pbmp_t logical_ports;
    bcm_port_t port, logical_port = -1;
    int stp_state = -1;
    int ii, nof_ports = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    SH_SAND_GET_PORT("port", logical_ports);

    BCM_PBMP_COUNT(logical_ports, nof_ports);

    if (nof_ports == 0)
    {
        LOG_CLI((BSL_META("Option port with a valid port-id is needed!\n")));
        SHR_EXIT();
    }
    if (stg_id == 0)
    {
        SHR_IF_ERR_EXIT(bcm_stg_default_get(unit, &stg_id));
    }

    BCM_PBMP_ITER(logical_ports, port)
    {
        logical_port = port;
        break;
    }

    SHR_IF_ERR_EXIT(bcm_stg_stp_get(unit, stg_id, logical_port, &stp_state));

    for (ii = 0; ii < BCM_STG_STP_COUNT; ii++)
    {
        if (sand_diag_stp_state_enum[ii].value == stp_state)
        {
            break;
        }
    }
    if (ii == BCM_STG_STP_COUNT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in getting STP state(%d)!\n", stp_state);
    }

    LOG_CLI((BSL_META("The STP state in stg %d is: %s (%d)\n"), stg_id, sand_diag_stp_state_enum[ii].string,
             sand_diag_stp_state_enum[ii].value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Add a vid to the STG.
 *  CMD format: stg vlan add [id=<stg-id>] VID=<vsi-id>
 *
 * \see
 *  bcm_stg_vlan_add()
 */
static shr_error_e
cmd_dnx_stg_vlan_add(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vid = BCM_VLAN_NONE;
    bcm_stg_t stg_id = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    SH_SAND_GET_UINT32("vid", vid);

    SHR_IF_ERR_EXIT(bcm_stg_vlan_add(unit, stg_id, vid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Remove a vid from the STG. If vid is BCM_VLAN_ALL, all VIDs will be removed from this stg.
 *  CMD format: stg vlan remove id=<stg-id> VID=<vsi-id>
 *
 * \see
 *  bcm_stg_vlan_remove()
 *  bcm_stg_vlan_remove_all()
 */
static shr_error_e
cmd_dnx_stg_vlan_remove(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vid = BCM_VLAN_NONE;
    bcm_stg_t stg_id = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);
    SH_SAND_GET_UINT32("vid", vid);

    if (vid == BCM_VLAN_ALL)
    {
        SHR_IF_ERR_EXIT(bcm_stg_vlan_remove_all(unit, stg_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_stg_vlan_remove(unit, stg_id, vid));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the default stg.
 *  CMD format: stg default set id=<stg-id>
 *
 * \see
 *  bcm_stg_default_set()
 */
static shr_error_e
cmd_dnx_stg_default_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", stg_id);

    SHR_IF_ERR_EXIT(bcm_stg_default_set(unit, stg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the default stg.
 *  CMD format: stg default get
 *
 * \see
 *  bcm_stg_default_get()
 */
static shr_error_e
cmd_dnx_stg_default_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_stg_t stg_id = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_stg_default_get(unit, &stg_id));
    LOG_CLI((BSL_META("The default Spaning Tree Group ID is: %d\n"), stg_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * cmd implementation end }
 */

/*
 * cmd definition begin {
 */
/**
 * DNX STG diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* {*/
/*
 * STG sub cmds definition begin {
 */
/** Operation on STP state begin {*/
static sh_sand_option_t dnx_stg_stp_set_options[] = {
    /**Keyword Type                  Description              Default-value  ext-info*/
    {"ID",     SAL_FIELD_TYPE_INT32, "Spanning Tree Group ID", NULL},
    {"port",   SAL_FIELD_TYPE_PORT,  "Port ID or port name",  NULL},
    {"state",  SAL_FIELD_TYPE_ENUM,  "STP state",             "forward",     (void *) sand_diag_stp_state_enum},
    {NULL}
};

static sh_sand_man_t dnx_stg_stp_set_man = {
    .brief    = "Set the stp state for a port in the Spanning Tree Group",
    .full     = "Set the stp state for a port in the Spanning Tree Group."
                 "possible states are forward/block/disable/learn/listen. If no state is provided, forward is used.",
    .synopsis = "id=<stg-id> port=<eth-ports> [state=<stp-state>]\n",
    .examples = "id=1 port=xe13 state=forward"
};

static sh_sand_option_t dnx_stg_stp_get_options[] = {
    /**Keyword Type                  Description              Default-value*/
    {"ID",     SAL_FIELD_TYPE_INT32, "Spanning Tree Group ID", "0"},
    {"port",   SAL_FIELD_TYPE_PORT,  "Port ID or port name",  NULL},
    {NULL}
};

static sh_sand_man_t dnx_stg_stp_get_man = {
    .brief    = "Get the stp state for a port in the Spanning Tree Group",
    .full     = "Get the stp state for a port in the Spanning Tree Group."
                "If no stg-id is provided, the state will be gotten from default stg.",
    .synopsis = "[id=<stg-id>] port=<eth-ports>\n",
    .examples = "id=1 port=xe13"
};

sh_sand_cmd_t sh_dnx_stg_stp_cmds[] = {
    /**keyword,   action,                sub-command,    options,                   man                 */
    {"Set",       cmd_dnx_stg_stp_set,   NULL,           dnx_stg_stp_set_options,   &dnx_stg_stp_set_man},
    {"Get",       cmd_dnx_stg_stp_get,   NULL,           dnx_stg_stp_get_options,   &dnx_stg_stp_get_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_stg_stp_man = {
    .brief    = "Operation on the STP state for a port in the Spanning Tree Group"
};
/** Operation on STP state end }*/

/** Operation on vlan begin {*/
static sh_sand_option_t dnx_stg_vlan_options[] = {
    /**Keyword Type                   Description              Default-value*/
    {"ID",     SAL_FIELD_TYPE_INT32,  "Spanning Tree Group ID", NULL},
    {"vid",    SAL_FIELD_TYPE_UINT32, "VSI-ID", NULL},
    {NULL}
};

static sh_sand_man_t dnx_stg_vlan_add_man = {
    .brief    = "Add the VID to the Spanning Tree Group",
    .full     = NULL,
    .synopsis = "id=<stg-id> vid=<vsi-id>\n",
    .examples = "id=1 vid=1"
};

static sh_sand_man_t dnx_stg_vlan_remove_man = {
    .brief    = "Remove the VID from the Spanning Tree Group. Use 0xFFFF for all VIDs.",
    .full     = NULL,
    .synopsis = "id=<stg-id> vid=<vsi-id>\n",
    .examples = "id=1 vid=1"
};

sh_sand_cmd_t sh_dnx_stg_vlan_cmds[] = {
    /**keyword,   action,                 sub-command,    options,                 man                   */
    {"Add",       cmd_dnx_stg_vlan_add,   NULL,           dnx_stg_vlan_options,    &dnx_stg_vlan_add_man   },
    {"ReMoVe",    cmd_dnx_stg_vlan_remove,NULL,           dnx_stg_vlan_options,    &dnx_stg_vlan_remove_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_stg_vlan_man = {
    .brief    = "Operations(add/remove) on the VLANs in a Spanning Tree Group"
};
/** Operation on vlan end }*/

/** Operation on default stg begin {*/
static sh_sand_option_t dnx_stg_default_set_options[] = {
    /**Keyword Type                  Description              Default-value*/
    {"ID",     SAL_FIELD_TYPE_INT32, "Spaning Tree Group ID", NULL},
    {NULL}
};

static sh_sand_man_t dnx_stg_default_set_man = {
    .brief    = "Set the default Spaning Tree Group ID",
    .full     = NULL,
    .synopsis = "id=<stg-id>\n",
    .examples = "id=1"
};

static sh_sand_man_t dnx_stg_default_get_man = {
    .brief    = "Get the default Spanning Tree Group ID",
    .full     = NULL,
    .synopsis = NULL,
    .examples = NULL
};

sh_sand_cmd_t sh_dnx_stg_default_cmds[] = {
    /**keyword,   action,                   sub-command,    options,                       man                       */
    {"Set",       cmd_dnx_stg_default_set,  NULL,           dnx_stg_default_set_options,   &dnx_stg_default_set_man},
    {"Get",       cmd_dnx_stg_default_get,  NULL,           NULL,                          &dnx_stg_default_get_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_stg_default_man = {
    .brief    = "Operations(set/get) on the default Spanning Tree Group ID"
};
/** Operation on default stg end }*/
/*
 * STG sub cmds definition end }
 */

/*
 * STG cmds definition begin {
 */
/** Operation on creating stg begin {*/
static sh_sand_option_t sh_dnx_stg_create_options[] = {
    /**Keyword Type                  Description              Default-value*/
    {"ID",     SAL_FIELD_TYPE_INT32, "Spanning Tree Group ID", "0"},
    {NULL}
};

static sh_sand_man_t sh_dnx_stg_create_man = {
    .brief    = "Create a Spanning Tree Group",
    .full     = "Create a Spanning Tree Group. If a valid group ID is provided, use it, else allocate a group ID internally.",
    .synopsis = "[id=<stg-id>]\n",
    .examples = "\n"
                "id=10"
};
/** Operation on creating stg end }*/

/** Operation on destroying stg begin {*/
static sh_sand_option_t sh_dnx_stg_destroy_options[] = {
    /**Keyword  Type                  Description              Default-value*/
    {"ID",      SAL_FIELD_TYPE_INT32, "Spaning Tree Group ID", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_stg_destroy_man = {
    .brief    = "Destroy a Spanning Tree Group with the IDValid Range is [1,256].",
    .full     = NULL,
    .synopsis = "id=<stg-id>",
    .examples = "id=10"
};
/** Operation on destroying stg end }*/

/** Operation on clearing stg begin {*/
static sh_sand_man_t sh_dnx_stg_clear_man = {
    .brief    = "Clear the STG model and reset it to initial state"
};
/** Operation on clearing stg end }*/

/*
 * STG cmds definition end }
 */

/*
 * STG cmds list begin {
 */
sh_sand_cmd_t sh_dnx_stg_cmds[] = {
    /**keyword,  action,                sub-command,               options,                    man                  */
    {"CReate",   cmd_dnx_stg_create,    NULL,                      sh_dnx_stg_create_options,  &sh_dnx_stg_create_man},
    {"DeSTRoy",  cmd_dnx_stg_destroy,   NULL,                      sh_dnx_stg_destroy_options, &sh_dnx_stg_destroy_man},
    {"stp",      NULL,                  sh_dnx_stg_stp_cmds,       NULL,                       &sh_dnx_stg_stp_man},
    {"VLan",     NULL,                  sh_dnx_stg_vlan_cmds,      NULL,                       &sh_dnx_stg_vlan_man},
    {"DeFaulT",  NULL,                  sh_dnx_stg_default_cmds,   NULL,                       &sh_dnx_stg_default_man},
    {"CLear",    cmd_dnx_stg_clear,     NULL,                      NULL,                       &sh_dnx_stg_clear_man},
    {NULL}
};

sh_sand_man_t sh_dnx_stg_man = {
    .brief    = "Diagnostic pack for stg module"
};
/*
 * STG cmds definition end }
 */
/* *INDENT-ON* }*/
/*
 * cmd definition end }
 */
