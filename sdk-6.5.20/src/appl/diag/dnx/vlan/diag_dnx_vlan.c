/** \file diag_dnx_vlan.c
 *
 * Main diagnostics for vlan applications
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * {
 */
/*************
 * INCLUDES  *
 */
/** appl */
/** allow drv.h include excplictly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** needed for FORMAT_PBMP_MAX */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/parse.h>

/** soc */
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dbal/dbal.h>

/** bcm */
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>

/** sal */
#include <sal/appl/sal.h>

/** local */
#include "diag_dnx_vlan.h"
#define NUM_OF_VIDS_PER_ROW (20)
/*
 * vlan show command functions
 */
shr_error_e
sh_dnx_vlan_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;
    bcm_vlan_data_t *list;
    char *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;
    int count, i, mode;
    bcm_port_t port;
    bcm_pbmp_t logical_ports;

    char port_name_buf[SOC_PBMP_FMT_LEN * BCM_TELEMETRY_PORT_NAME_SIZE];
    int port_name_buf_index, port_name_len;
    char *port_name;
    char bufp[FORMAT_PBMP_MAX], bufu[FORMAT_PBMP_MAX];
    char pfmtp[SOC_PBMP_FMT_LEN];
    char pfmtu[SOC_PBMP_FMT_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_INT32("mode", mode);

    SHR_IF_ERR_EXIT(bcm_vlan_list(unit, &list, &count));
    PRT_TITLE_SET("vlan show");
    if (!mode)
    {
        PRT_COLUMN_ADD("Vlan");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_BINARY, "ports");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_BINARY, "untagged");
        for (i = 0; i < count; i++)
        {
            if (vlan == 0 || list[i].vlan_tag == vlan)
            {
                bcm_vlan_mcast_flood_t mode = BCM_VLAN_MCAST_FLOOD_COUNT;
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                format_bcm_pbmp(unit, bufp, sizeof(bufp), list[i].port_bitmap);
                format_bcm_pbmp(unit, bufu, sizeof(bufu), list[i].ut_port_bitmap);
                PRT_CELL_SET("%d", list[i].vlan_tag);
                PRT_CELL_SET("%s (%s)", bufp, SOC_PBMP_FMT(list[i].port_bitmap, pfmtp));
                PRT_CELL_SET("%s (%s) %s", bufu, SOC_PBMP_FMT(list[i].ut_port_bitmap, pfmtu),
                             bcm_vlan_mcast_flood_str[mode]);
            }
        }
    }
    else if (count <= NUM_OF_VIDS_PER_ROW || vlan != 0)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_BINARY, "ports");
        for (i = 0; i < count; i++)
        {
            if (vlan != 0 && list[i].vlan_tag != vlan)
            {
                continue;
            }
            PRT_COLUMN_ADD("%d", list[i].vlan_tag);
            PRT_COLUMN_ALIGN;
        }
        SH_SAND_GET_PORT("pbmp", logical_ports);
        BCM_PBMP_ITER(logical_ports, port)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s(%d)", BCM_PORT_NAME(unit, port), port);
            for (i = 0; i < count; i++)
            {
                if (vlan != 0 && list[i].vlan_tag != vlan)
                {
                    continue;
                }
                if (BCM_PBMP_MEMBER(list[i].ut_port_bitmap, port))
                {
                    PRT_CELL_SET("%-4s", "Y(U)");
                }
                else if (BCM_PBMP_MEMBER(list[i].port_bitmap, port))
                {
                    PRT_CELL_SET("%-4s", "Y");
                }
            }
        }
    }
    else
    {
        PRT_COLUMN_ADD("Vlan");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ports");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Untagged Ports");
        for (i = 0; i < count; i++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%-4d", list[i].vlan_tag);
            port_name_buf_index = 0;
            port_name_buf[port_name_buf_index] = '\0';
            BCM_PBMP_ITER(list[i].port_bitmap, port)
            {
                port_name = BCM_PORT_NAME(unit, port);
                port_name_len = sal_strlen(port_name);
                if (port_name_buf_index + port_name_len < SOC_PBMP_FMT_LEN * BCM_TELEMETRY_PORT_NAME_SIZE - 1)
                {
                    sal_strncpy(port_name_buf + port_name_buf_index, port_name, port_name_len);
                    port_name_buf_index += port_name_len;
                    sal_strncpy_s(port_name_buf + port_name_buf_index, ",",
                                  SOC_PBMP_FMT_LEN * BCM_TELEMETRY_PORT_NAME_SIZE - port_name_buf_index);
                    port_name_buf_index += 1;
                }
                port_name_buf[port_name_buf_index] = '\0';
            }
            PRT_CELL_SET("%s", port_name_buf);
            port_name_buf_index = 0;
            port_name_buf[port_name_buf_index] = '\0';
            BCM_PBMP_ITER(list[i].ut_port_bitmap, port)
            {
                port_name = BCM_PORT_NAME(unit, port);
                port_name_len = sal_strlen(port_name);
                if (port_name_buf_index + port_name_len < SOC_PBMP_FMT_LEN * BCM_TELEMETRY_PORT_NAME_SIZE - 1)
                {
                    sal_strncpy(port_name_buf + port_name_buf_index, port_name, port_name_len);
                    port_name_buf_index += port_name_len;
                    sal_strncpy_s(port_name_buf + port_name_buf_index, ",",
                                  SOC_PBMP_FMT_LEN * BCM_TELEMETRY_PORT_NAME_SIZE - port_name_buf_index);
                    port_name_buf_index += 1;
                }
                port_name_buf[port_name_buf_index] = '\0';
            }
            PRT_CELL_SET("%s", port_name_buf);
        }
    }
    PRT_COMMITX;
    SHR_IF_ERR_EXIT(bcm_vlan_list_destroy(unit, list, count));
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * vlan create command functions
 */
shr_error_e
sh_dnx_vlan_create_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;
    bcm_pbmp_t portbitmap;
    bcm_pbmp_t untagbitmap;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_CLEAR(portbitmap);
    BCM_PBMP_CLEAR(untagbitmap);

    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_PORT("portbitmap", portbitmap);
    SH_SAND_GET_PORT("untagbitmap", untagbitmap);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vlan));
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vlan, portbitmap, untagbitmap));
exit:
    SHR_FUNC_EXIT;
}

/*
 * vlan destroy command functions
 */
shr_error_e
sh_dnx_vlan_destroy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("vlan", vlan);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    SHR_IF_ERR_EXIT(bcm_vlan_destroy(unit, vlan));
exit:
    SHR_FUNC_EXIT;
}

/*
 * vlan clear command functions
 */
shr_error_e
sh_dnx_vlan_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_vlan_destroy_all(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * vlan add command functions
 */
shr_error_e
sh_dnx_vlan_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;
    bcm_pbmp_t portbitmap;
    bcm_pbmp_t untagbitmap;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_CLEAR(portbitmap);
    BCM_PBMP_CLEAR(untagbitmap);

    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_PORT("portbitmap", portbitmap);
    SH_SAND_GET_PORT("untagbitmap", untagbitmap);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vlan, portbitmap, untagbitmap));
exit:
    SHR_FUNC_EXIT;
}

/*
 * vlan remove command functions
 */
shr_error_e
sh_dnx_vlan_remove_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;
    bcm_pbmp_t portbitmap;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_CLEAR(portbitmap);

    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_PORT("portbitmap", portbitmap);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    SHR_IF_ERR_EXIT(bcm_vlan_port_remove(unit, vlan, portbitmap));
exit:
    SHR_FUNC_EXIT;
}

/*
 * vlan default command functions
 */
shr_error_e
sh_dnx_vlan_default_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("vlan", vlan);

    if (vlan == 0)
    {
        SHR_IF_ERR_EXIT(bcm_vlan_default_get(unit, &vlan));
        cli_out("Default VLAN ID is %d\n", vlan);
    }
    else
    {
        BCM_DNX_VLAN_CHK_ID(unit, vlan);
        SHR_IF_ERR_EXIT(bcm_vlan_default_set(unit, vlan));
        cli_out("Default VLAN ID set to %d\n", vlan);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * vlan gport command functions
 */
shr_error_e
sh_dnx_vlan_gport_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_vlan_t vlan;
    int r = _SHR_E_NONE;
    int is_untagged = 0;
    char *action;
    uint32 nof_ports;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_STR("action", action);
    SH_SAND_GET_PORT("port", logical_ports);
    BCM_DNX_VLAN_CHK_ID(unit, vlan);

    if ((sal_strcasecmp(action, "delete") != 0) &&
        (sal_strcasecmp(action, "add") != 0) &&
        (sal_strcasecmp(action, "get") != 0) &&
        (sal_strcasecmp(action, "clear") != 0) && (sal_strcasecmp(action, "show") != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s: ERROR: Invalid gport subcommand\n", action);
    }
    if ((sal_strcasecmp(action, "clear") != 0) && (sal_strcasecmp(action, "show") != 0))
    {
        /*
         * "clear" and "show" do not need a gport parameter, the others do 
         */
        BCM_PBMP_COUNT(logical_ports, nof_ports);
        if (nof_ports == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "It is expected port to be configured!\n");
        }
    }
    if (sal_strcasecmp(action, "show") == 0)
    {
        int i, count;
        bcm_gport_t *port_array = NULL;
        int *is_untagged_array = NULL;

        port_array = sal_alloc(sizeof(bcm_gport_t) * BCM_PBMP_PORT_MAX, "sh_dnx_vlan_gport_cmd.port_array");
        if (port_array == NULL)
        {
            cli_out("Memory allocation failure\n");
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Memory allocation failure");
        }
        is_untagged_array = sal_alloc(sizeof(int) * BCM_PBMP_PORT_MAX, "sh_dnx_vlan_gport_cmd.port_array");
        if (is_untagged_array == NULL)
        {
            cli_out("Memory allocation failure\n");
            sal_free(port_array);
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Memory allocation failure");
        }
        /*
         * Get all virtual and physical ports from the specified VLAN 
         */
        
        if ((r = bcm_vlan_gport_get_all(unit, vlan, BCM_PBMP_PORT_MAX, port_array, is_untagged_array, &count)) < 0)
        {
            sal_free(port_array);
            sal_free(is_untagged_array);
        SHR_ERR_EXIT(r, "bcm_vlan_gport_get_all")}
        /*
         * Count will always be non-zero (BCM_E_NOT_FOUND returned when
         * VLAN is valid but has no associated ports.
         */
        cli_out("Virtual and physical ports defined for VLAN: %d\n", vlan);
        for (i = 0; i < count; i++)
        {
            /*
             * port name is empty string if port is not local 
             */
            const char *port_name = mod_port_name(unit, -1, port_array[i]);
            if (*port_name)
            {
                cli_out("  %4s (%stagged)\n", port_name, is_untagged_array[i] ? "un" : "");
            }
        }
        sal_free(port_array);
        sal_free(is_untagged_array);
    }
    else
    {
        BCM_PBMP_ITER(logical_ports, port)
        {
            if (sal_strcasecmp(action, "delete") == 0)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_gport_delete(unit, vlan, port));
            }
            else if (sal_strcasecmp(action, "add") == 0)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vlan, port, 0));
            }
            else if (sal_strcasecmp(action, "get") == 0)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_gport_get(unit, vlan, port, &is_untagged));
            }
            else if (sal_strcasecmp(action, "clear") == 0)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_gport_delete_all(unit, vlan));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Options list for 'vlan show' shell command
 */
static sh_sand_option_t vlan_show_options[] = {
    /*
     * Name | Type | Description | Default
     */
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN id", "0", NULL, "1-4095", SH_SAND_ARG_FREE},
    {"mode", SAL_FIELD_TYPE_INT32, "mode", "0", NULL, "0-1"},
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}      /* End of options list - must be last. */
};
static sh_sand_option_t vlan_default_options[] = {
    /*
     * Name | Type | Description | Default
     */
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN id", "0", NULL, "1-4095", SH_SAND_ARG_FREE},
    {NULL}      /* End of options list - must be last. */
};
static sh_sand_option_t vlan_destroy_options[] = {
    /*
     * Name | Type | Description | Default
     */
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN id", "-1", NULL, "1-4095", SH_SAND_ARG_FREE},
    {NULL}      /* End of options list - must be last. */
};
static sh_sand_option_t vlan_command_options[] = {
    /*
     * Name | Type | Description | Default
     */
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN id", "-1", NULL, "1-4095", SH_SAND_ARG_FREE},
    {"portbitmap", SAL_FIELD_TYPE_PORT, "portbitmap or port id", "none"},
    {"untagbitmap", SAL_FIELD_TYPE_PORT, "untagbitmap or port id", "none"},
    {NULL}      /* End of options list - must be last. */
};
static sh_sand_option_t vlan_gport_options[] = {
    /*
     * Name | Type | Description | Default
     */
    {"action", SAL_FIELD_TYPE_STR, "vlan gport action", "none", NULL, NULL, SH_SAND_ARG_FREE},
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN id", "-1", NULL, "1-4095"},
    {"port", SAL_FIELD_TYPE_PORT, "port", "none"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief - Man page for 'vlan create/destroy/add/remove/gport/default' shell command
 */
static sh_sand_man_t vlan_show_man = {
    .brief = "Show vlan info",
    .examples = "" "vlan=1\n" "mode=1\n"
};
static sh_sand_man_t vlan_create_man = {
    .brief = "Create vlan",
    .examples = "vlan=2\n" "vlan=3 portbitmap=203\n" "vlan=4 untagbitmap=203"
};
static sh_sand_man_t vlan_destroy_man = {
    .brief = "Destroy vlan",
    .examples = "vlan=2\n" "vlan=3\n" "vlan=4"
};
static sh_sand_man_t vlan_clear_man = {
    .brief = "Destroy all VLANs",
    .examples = ""
};
static sh_sand_man_t vlan_add_man = {
    .brief = "Add vlan",
    .examples = "vlan=1\n" "vlan=1 portbitmap=203\n" "vlan=2 untagbitmap=203"
};
static sh_sand_man_t vlan_remove_man = {
    .brief = "Remove ports from a VLAN",
    .examples = "vlan=1\n" "vlan=1 portbitmap=203\n" "vlan=2 untagbitmap=203"
};
static sh_sand_man_t vlan_gport_man = {
    .brief = "Gport vlan",
    .examples = "add vlan=1 Port=203"
};
static sh_sand_man_t vlan_default_man = {
    .brief = "change default vlan",
    .examples = "vlan=1\n"
};
/**
 * \brief DNX VLAN diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for VLAN diagnostic commands
 */
sh_sand_cmd_t sh_dnx_vlan_cmds[] = {
    /*
     * Name | Leaf Action | Node | Options for Leaf | Usage 
     */
    {"show", sh_dnx_vlan_show_cmd, NULL, vlan_show_options, &vlan_show_man},
    {"create", sh_dnx_vlan_create_cmd, NULL, vlan_command_options, &vlan_create_man},
    {"destroy", sh_dnx_vlan_destroy_cmd, NULL, vlan_destroy_options, &vlan_destroy_man},
    {"clear", sh_dnx_vlan_clear_cmd, NULL, NULL, &vlan_clear_man},
    {"add", sh_dnx_vlan_add_cmd, NULL, vlan_command_options, &vlan_add_man},
    {"remove", sh_dnx_vlan_remove_cmd, NULL, vlan_command_options, &vlan_remove_man},
    {"gport", sh_dnx_vlan_gport_cmd, NULL, vlan_gport_options, &vlan_gport_man},
    {"default", sh_dnx_vlan_default_cmd, NULL, vlan_default_options, &vlan_default_man},
    {NULL}
};
/**
 * \brief - Man page for 'vlan' shell command
 */
sh_sand_man_t sh_dnx_vlan_man = {
    .brief = "VLAN diagnostic commands"
};

/*
 * }
 */
