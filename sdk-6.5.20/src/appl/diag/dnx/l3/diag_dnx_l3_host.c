/** \file diag_dnx_l3_host.c
 *
 * file for l3 host diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** bcm */
#include <bcm/l3.h>
/** soc */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_structures.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"

#include <shared/utilex/utilex_framework.h>
/*
 * }
 */

/**
 * Callback function for testing the host diagnostics commands.
 */
static shr_error_e
dnx_l3_host_cb(
    int unit)
{
    bcm_ip_t ip_address = 0xC0A80101;
    bcm_ip6_t ip6_address = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    int fec = 10;
    uint32 vrf = 1;
    bcm_l3_host_t host_entry;

    SHR_FUNC_INIT_VARS(unit);

    /** "ip=192.168.1.1 fec=10 vrf=1" */
    bcm_l3_host_t_init(&host_entry);
    host_entry.l3a_ip_addr = ip_address;
    host_entry.l3a_intf = fec;
    host_entry.l3a_vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host_entry));

    /** ip6=1111:1111:1111:1111:0000:0000:0000:0000 v6=1 vrf=2 */
    vrf = 2;
    bcm_l3_host_t_init(&host_entry);
    host_entry.l3a_flags = BCM_L3_IP6;
    sal_memcpy(host_entry.l3a_ip6_addr, ip6_address, sizeof(bcm_ip6_t));
    host_entry.l3a_intf = fec;
    host_entry.l3a_vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host_entry));
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_l3_host_add_man = {
    .brief = "Add an UC host entry\n",
    .full = "Add an IPv4 or an IPv6 host entry to the UC host table using an IP as key and a FEC as result\n",
    .synopsis = "ip=<IPv4 IP>/ip6=<IPv6 IP>, [vrf=<VRF>], [fec=<FEC>]",
    .examples = "ip=192.168.1.1 fec=10 vrf=1" "\n" "ip6=1111:1111:1111:1111:0000:0000:0000:0000 v6=1 vrf=2 fec=20"
};

static sh_sand_man_t dnx_l3_host_delete_man = {
    .brief = "Delete an UC host entry\n",
    .full = "Delete an IPv4 or an IPv6 host entry from the UC host table providing its IP.\n",
    .init_cb = dnx_l3_host_cb,
    .synopsis = "ip=<IPv4 IP>/ip6=<IPv6 IP>, [vrf=<VRF>]",
    .examples = "ip=192.168.1.1 vrf=1" "\n" "ip6=1111:1111:1111:1111:0000:0000:0000:0000 vrf=2 v6=1"
};

static sh_sand_man_t dnx_l3_host_update_man = {
    .brief = "Update an UC host entry\n",
    .full = "Update an IPv4 or IPv6 UC host entry providing in the UC host table, proividing its IP and new FEC\n",
    .init_cb = dnx_l3_host_cb,
    .synopsis = "ip=<IPv4 IP>/ip6=<IPv6 IP>, [vrf=<VRF>], [fec=<FEC>]",
    .examples = "ip=192.168.1.1 fec=11 vrf=1" "\n" "ip6=1111:1111:1111:1111:0000:0000:0000:0000 vrf=2 v6=1 fec=21"
};

static sh_sand_man_t dnx_l3_host_find_man = {
    .brief = "Find the result of an UC host entry",
    .full = "Find the FEC result of an IPv4 or IPv6 entry in the UC host table, providing its IP.",
    .init_cb = dnx_l3_host_cb,
    .synopsis = "ip=<IPv4 IP>/ip6=<IPv6 IP>, [vrf=<VRF>]",
    .examples = "ip=192.168.1.1 vrf=1" "\n" "ip6=1111:1111:1111:1111:0000:0000:0000:0000 vrf=2 v6=1"
};

static sh_sand_option_t dnx_l3_host_add_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"fec", SAL_FIELD_TYPE_INT32, "FEC", "0"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_l3_host_delete_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_l3_host_update_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"fec", SAL_FIELD_TYPE_INT32, "FEC", "0"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_l3_host_find_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed host entry.
 *    It is valid for add/delete/find/update commands.
 * \param [in] unit - unit ID
 * \param [in] host_info - The structure that uniquely describes the accessed entry
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * add
 *          * delete
 *          * destroy
 *          * find
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_host_entry_print(
    int unit,
    bcm_l3_host_t * host_info,
    sh_sand_control_t * sand_control,
    char *action)
{
    int ip_version = 4;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Host entry %s", action);

    PRT_COLUMN_ADD("IP version");
    PRT_COLUMN_ADD("VRF");
    PRT_COLUMN_ADD("IP");
    PRT_COLUMN_ADD("FEC destination");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Set the value of the ip_version based on the IP6 flag */
    if (_SHR_IS_FLAG_SET(host_info->l3a_flags, BCM_L3_IP6))
    {
        ip_version = 6;
    }

    PRT_CELL_SET("%d", ip_version);
    PRT_CELL_SET("0x%04x", host_info->l3a_vrf);

    /** Receive the value of the IP address in the buffer string variable. */
    if (ip_version == 4)
    {
        SHR_IF_ERR_EXIT(utilex_pp_ip_addr_numeric_to_string(host_info->l3a_ip_addr, 1, buffer));
    }
    else
    {
        sal_sprintf(buffer, "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
                    host_info->l3a_ip6_addr[0], host_info->l3a_ip6_addr[1], host_info->l3a_ip6_addr[2],
                    host_info->l3a_ip6_addr[3], host_info->l3a_ip6_addr[4], host_info->l3a_ip6_addr[5],
                    host_info->l3a_ip6_addr[6], host_info->l3a_ip6_addr[7], host_info->l3a_ip6_addr[8],
                    host_info->l3a_ip6_addr[9], host_info->l3a_ip6_addr[10], host_info->l3a_ip6_addr[11],
                    host_info->l3a_ip6_addr[12], host_info->l3a_ip6_addr[13], host_info->l3a_ip6_addr[14],
                    host_info->l3a_ip6_addr[15]);
    }
    PRT_CELL_SET("%s", buffer);

    PRT_CELL_SET("0x%04x", host_info->l3a_intf);

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an IPv4 or IPv6 host entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_host_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    int fec;
    int v6_flag;
    uint32 vrf;
    int ip_present, ip6_present;
    bcm_l3_host_t host_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_INT32("fec", fec);
    SH_SAND_GET_BOOL("v6", v6_flag);

    bcm_l3_host_t_init(&host_entry);
    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 addres together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        host_entry.l3a_flags = BCM_L3_IP6;
        sal_memcpy(host_entry.l3a_ip6_addr, ip6_address, sizeof(bcm_ip6_t));
    }
    else
    {
        host_entry.l3a_ip_addr = ip_address;
    }

    host_entry.l3a_intf = fec;
    host_entry.l3a_vrf = vrf;

    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host_entry));
    SHR_IF_ERR_EXIT(bcm_l3_host_find(unit, &host_entry));
    SHR_IF_ERR_EXIT(dnx_l3_host_entry_print(unit, &host_entry, sand_control, "add"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes an existing IPv4 or IPv6 host entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_host_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    uint32 vrf;
    int v6_flag;
    int ip_present, ip6_present;
    bcm_l3_host_t host_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_BOOL("v6", v6_flag);

    bcm_l3_host_t_init(&host_entry);
    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 addres together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        host_entry.l3a_flags = BCM_L3_IP6;
        sal_memcpy(host_entry.l3a_ip6_addr, ip6_address, sizeof(bcm_ip6_t));
    }
    else
    {
        host_entry.l3a_ip_addr = ip_address;
    }
    host_entry.l3a_vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_l3_host_find(unit, &host_entry));
    SHR_IF_ERR_EXIT(bcm_l3_host_delete(unit, &host_entry));
    SHR_IF_ERR_EXIT(dnx_l3_host_entry_print(unit, &host_entry, sand_control, "delete"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that updates an existing IPv4 or IPv6 host entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_host_update_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    int fec;
    uint32 vrf;
    int v6_flag;
    int ip_present, ip6_present;
    bcm_l3_host_t host_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_INT32("fec", fec);
    SH_SAND_GET_BOOL("v6", v6_flag);

    bcm_l3_host_t_init(&host_entry);
    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 addres together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        host_entry.l3a_flags = BCM_L3_IP6;
        sal_memcpy(host_entry.l3a_ip6_addr, ip6_address, sizeof(bcm_ip6_t));
    }
    else
    {
        host_entry.l3a_ip_addr = ip_address;
    }
    host_entry.l3a_intf = fec;
    host_entry.l3a_vrf = vrf;
    host_entry.l3a_flags |= BCM_L3_REPLACE;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host_entry));
    SHR_IF_ERR_EXIT(bcm_l3_host_find(unit, &host_entry));
    SHR_IF_ERR_EXIT(dnx_l3_host_entry_print(unit, &host_entry, sand_control, "update"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that finds an existing IPv4 or IPv6 host entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_host_find_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    uint32 vrf;
    int v6_flag;
    int ip_present, ip6_present;
    bcm_l3_host_t host_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_BOOL("v6", v6_flag);

    bcm_l3_host_t_init(&host_entry);
    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 addres together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        host_entry.l3a_flags = BCM_L3_IP6;
        sal_memcpy(host_entry.l3a_ip6_addr, ip6_address, sizeof(bcm_ip6_t));
    }
    else
    {
        host_entry.l3a_ip_addr = ip_address;
    }
    host_entry.l3a_vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_l3_host_find(unit, &host_entry));
    SHR_IF_ERR_EXIT(dnx_l3_host_entry_print(unit, &host_entry, sand_control, "find"));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_host_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"add", sh_dnx_l3_host_add_cmd, NULL, dnx_l3_host_add_options, &dnx_l3_host_add_man}
    ,
    {"find", sh_dnx_l3_host_find_cmd, NULL, dnx_l3_host_find_options, &dnx_l3_host_find_man}
    ,
    {"update", sh_dnx_l3_host_update_cmd, NULL, dnx_l3_host_update_options, &dnx_l3_host_update_man}
    ,
    {"delete", sh_dnx_l3_host_delete_cmd, NULL, dnx_l3_host_delete_options, &dnx_l3_host_delete_man}
    ,
    {NULL}
};
