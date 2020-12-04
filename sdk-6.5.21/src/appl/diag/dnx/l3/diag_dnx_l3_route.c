/** \file diag_dnx_l3_route.c
 *
 * file for l3 route diagnostics
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
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"
/** bcm */
#include <bcm/ipmc.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */

/**
 * Callback function for testing the route diagnostics commands.
 */
static shr_error_e
dnx_l3_route_cb(
    int unit)
{
    bcm_ip_t ip_address = 0xC0A80101;
    bcm_ip6_t ip6_address = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0, 0, 0, 0, 0, 0, 0, 0 };
    bcm_ip_t ip_mask = 0xFFFF0000;
    bcm_ip6_t ip6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0 };
    int fec = 0xccce;
    uint32 vrf = 10;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_multicast_t mc_group = 50;
    bcm_vlan_t vid = 100;
    bcm_ip_t mc_ip_addr = 0xe0a80101;
    bcm_ip_t mc_ip_mask = 0xFFFFFFFF;
    bcm_ip6_t mc_ip6_addr = { 0xFF, 0xFF, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    bcm_ip6_t mc_ip6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** vrf=10 ip=192.168.1.1 mask=255.255.0.0 fec=0xccce */
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_intf = fec;
    route_entry.l3a_vrf = vrf;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));

    /** ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 */
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_flags |= BCM_L3_IP6;
    sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
    sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
    route_entry.l3a_intf = fec;
    SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));

    /** vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1 mc_group=50*/
    vrf = 5;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.vid = vid;
    data.group = mc_group;
    data.vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &data));

    /** vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 raw=1 fec=500 */
    bcm_ipmc_addr_t_init(&data);
    data.flags |= BCM_IPMC_IP6;
    sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
    sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
    data.l3a_intf = 500;
    data.vrf = vrf;
    data.flags |= BCM_IPMC_RAW_ENTRY;
    SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &data));

    /** vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1 */
    ip_mask = 0xFFFFFF00;
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_vrf = 15;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    route_entry.l3a_intf = fec;
    if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));

    /** vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1" */
    ip_address = 0xC0A80201;
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_vrf = 15;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    route_entry.l3a_intf = fec;
    if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));

exit:
    SHR_FUNC_EXIT;
}

/** Deinit callback function for testing the route diagnostics commands.*/
static shr_error_e
dnx_l3_route_deinit_cb(
    int unit)
{
    bcm_ip_t ip_address = 0xC0A80101;
    bcm_ip6_t ip6_address = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0, 0, 0, 0, 0, 0, 0, 0 };
    bcm_ip_t ip_mask = 0xFFFF0000;
    bcm_ip6_t ip6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0 };
    uint32 vrf = 10;
    uint32 vid = 100;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_ip_t mc_ip_addr = 0xe0a80101;
    bcm_ip_t mc_ip_mask = 0xFFFFFFFF;
    bcm_ip6_t mc_ip6_addr = { 0xFF, 0xFF, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    bcm_ip6_t mc_ip6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** vrf=10 ip=192.168.1.1 mask=255.255.0.0 fec=0xccce */
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_vrf = vrf;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route_entry));

    /** ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 */
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_flags |= BCM_L3_IP6;
    sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
    sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
    SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route_entry));

    /** vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1 mc_group=50*/
    vrf = 5;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.vid = vid;
    data.vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_ipmc_remove(unit, &data));

    /** vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 raw=1 fec=500 */
    bcm_ipmc_addr_t_init(&data);
    data.flags |= BCM_IPMC_IP6;
    sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
    sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
    data.vrf = vrf;
    data.flags |= BCM_IPMC_RAW_ENTRY;
    SHR_IF_ERR_EXIT(bcm_ipmc_remove(unit, &data));

    /** vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1 */
    ip_mask = 0xFFFFFF00;
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_vrf = 15;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route_entry));

    /** vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1" */
    ip_address = 0xC0A80201;
    bcm_l3_route_t_init(&route_entry);
    route_entry.l3a_vrf = 15;
    route_entry.l3a_subnet = ip_address;
    route_entry.l3a_ip_mask = ip_mask;
    if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route_entry));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_l3_route_add_man = {
    .brief = "Add an UC route entry\n",
    .full = "Add an IPv4 or an IPv6 route entry to the UC route table using a masked IP as key and a FEC as result\n",
    .deinit_cb = dnx_l3_route_deinit_cb,
    .synopsis =
        "(ip=<IPv4 IP> mask=<IPv4 mask>) | (ip6=<IPv6 IP> ip6mask=<IPv6 mask>), [vrf=<VRF>], [fec=<FEC>], [v6=<NO>], [raw=<NO>], [multicast=<NO>], [kbp_fwd=<NO>], [kbp_rpf=<NO>]",
    .examples =
        "vrf=10 ip=192.168.1.1 mask=255.255.0.0 fec=10" "\n"
        "ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1" "\n"
        "vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1 mc_group=50" "\n"
        "vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 multicast=1 raw=1 fec=500"
        "\n" "vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1"
        "\n" "vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1"
};

static sh_sand_man_t dnx_l3_route_delete_man = {
    .brief = "Delete an UC route entry\n",
    .full = "Delete an IPv4 or an IPv6 route entry from the UC route table using its masked IP\n",
    .init_cb = dnx_l3_route_cb,
    .synopsis = "(ip=<IPv4 IP> mask=<IPv4 mask>) | (ip6=<IPv6 IP> ip6mask=<IPv6 mask>), [vrf=<VRF>]",
    .examples = "vrf=10 ip=192.168.1.1 mask=255.255.0.0" "\n"
        "ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1" "\n"
        "vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1" "\n"
        "vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 multicast=1"
        "\n" "vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1"
        "\n" "vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1"
};

static sh_sand_man_t dnx_l3_route_update_man = {
    .brief = "Update an UC route entry\n",
    .full =
        "Update an IPv4 or an IPv6 route entry in the UC route table using its masked IP as key and a new FEC as result\n",
    .init_cb = dnx_l3_route_cb,
    .deinit_cb = dnx_l3_route_deinit_cb,
    .synopsis = "(ip=<IPv4 IP> mask=<IPv4 mask>) | (ip6=<IPv6 IP> ip6mask=<IPv6 mask>), [vrf=<VRF>], [fec=<FEC>]",
    .examples = "vrf=10 ip=192.168.1.1 mask=255.255.0.0 raw=1 fec=500" "\n"
        "ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 fec=100" "\n"
        "vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1 mc_group=50" "\n"
        "vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 multicast=1 fec=10"
        "\n" "vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1"
        "\n" "vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1"
};

static sh_sand_man_t dnx_l3_route_get_man = {
    .brief = "Get an UC route entry\n",
    .full = "Get the FEC result of an IPv4 or an IPv6 route entry from the UC route table using its masked IP\n",
    .init_cb = dnx_l3_route_cb,
    .deinit_cb = dnx_l3_route_deinit_cb,
    .synopsis = "(ip=<IPv4 IP> mask=<IPv4 mask>) | (ip6=<IPv6 IP> ip6mask=<IPv6 mask>), [vrf=<VRF>]",
    .examples = "vrf=10 ip=192.168.1.1 mask=255.255.0.0" "\n"
        "ip6=1111:1111:1111:1111:0000:0000:0000:0000 ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1" "\n"
        "vrf=5 mc_ip=224.168.1.1 mc_mask=255.255.255.255 vid=100 multicast=1" "\n"
        "vrf=5 mc_ip6=FFFF:1111:1111:1111:0000:0000:0000:0000 mc_ip6mask=FFFF:FFFF:FFFF:FFFF:FFFF:0000:0000:0000 v6=1 multicast=1 raw=1"
        "\n" "vrf=15 ip=192.168.1.1 mask=255.255.255.0 kbp_fwd=1"
        "\n" "vrf=15 ip=192.168.2.1 mask=255.255.255.0 kbp_rpf=1"
};

static sh_sand_option_t dnx_l3_route_add_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"ip", SAL_FIELD_TYPE_IP4, "IPv4 address. For 'multicast', this is source ip addr", "0.0.0.0"},
    {"mask", SAL_FIELD_TYPE_IP4, "IPv4 mask. For 'multicast', this is source ip mask", "0.0.0.0"},
    {"ip6", SAL_FIELD_TYPE_IP6, "IPv6 address. For 'multicast', this is source ip addr",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask. For 'multicast', this is source ip mask",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"fec", SAL_FIELD_TYPE_INT32, "FEC", "0xcccf"},
    {"mc_group", SAL_FIELD_TYPE_INT32, "Multicast group id. Only relevant for 'multicast'.", "0"},
    {"vid", SAL_FIELD_TYPE_UINT32, "Entry ETH-RIF. Only relevant for 'multicast'.", "0"},
    {"mc_ip6", SAL_FIELD_TYPE_IP6, "IPv6 address. For 'multicast', this is destination ip addr",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask. For 'multicast', this is destination ip mask",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip", SAL_FIELD_TYPE_IP4, "IPv4 address. For 'multicast', this is destination ip addr", "0.0.0.0"},
    {"mc_mask", SAL_FIELD_TYPE_IP4, "IPv4 mask. For 'multicast', this is destination ip mask", "0.0.0.0"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {"raw", SAL_FIELD_TYPE_BOOL, "Raw flag. If set, no encoding for 'fec' (taken 'as is')", "FALSE"},
    {"multicast", SAL_FIELD_TYPE_BOOL, "Flag. If set, add a multicast entry", "FALSE"},
    {"kbp_fwd", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP forwarding route entry", "FALSE"},
    {"kbp_rpf", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP RPF route entry", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_l3_route_delete_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"mask", SAL_FIELD_TYPE_IP4, "IPv4 mask", "0.0.0.0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {"vid", SAL_FIELD_TYPE_UINT32, "Entry ETH-RIF. Only relevant for 'multicast'.", "0"},
    {"mc_ip6", SAL_FIELD_TYPE_IP6, "IPv6 address. For 'multicast', this is destination ip addr",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask. For 'multicast', this is destination ip mask",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip", SAL_FIELD_TYPE_IP4, "IPv4 address. For 'multicast', this is destination ip addr", "0.0.0.0"},
    {"mc_mask", SAL_FIELD_TYPE_IP4, "IPv4 mask. For 'multicast', this is destination ip mask", "0.0.0.0"},
    {"multicast", SAL_FIELD_TYPE_BOOL, "Flag. If set, add a multicast entry", "FALSE"},
    {"kbp_fwd", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP forwarding route entry", "FALSE"},
    {"kbp_rpf", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP RPF route entry", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_l3_route_get_options[] = {
    {"vrf", SAL_FIELD_TYPE_UINT32, "VRF", "0"},
    {"mask", SAL_FIELD_TYPE_IP4, "IPv4 mask", "0.0.0.0"},
    {"IP", SAL_FIELD_TYPE_IP4, "IPv4 address", "0.0.0.0"},
    {"IP6", SAL_FIELD_TYPE_IP6, "IPv6 address", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask", "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"v6", SAL_FIELD_TYPE_BOOL, "v6", "FALSE"},
    {"vid", SAL_FIELD_TYPE_UINT32, "Entry ETH-RIF. Only relevant for 'multicast'.", "0"},
    {"mc_ip6", SAL_FIELD_TYPE_IP6, "IPv6 address. For 'multicast', this is destination ip addr",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip6mask", SAL_FIELD_TYPE_IP6, "IPv6 mask. For 'multicast', this is destination ip mask",
     "0000:0000:0000:0000:0000:0000:0000:0000"},
    {"mc_ip", SAL_FIELD_TYPE_IP4, "IPv4 address. For 'multicast', this is destination ip addr", "0.0.0.0"},
    {"mc_mask", SAL_FIELD_TYPE_IP4, "IPv4 mask. For 'multicast', this is destination ip mask", "0.0.0.0"},
    {"multicast", SAL_FIELD_TYPE_BOOL, "Flag. If set, add a multicast entry", "FALSE"},
    {"raw", SAL_FIELD_TYPE_BOOL, "Raw flag. If set, no encoding for 'fec' (taken 'as is')", "FALSE"},
    {"kbp_fwd", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP forwarding route entry", "FALSE"},
    {"kbp_rpf", SAL_FIELD_TYPE_BOOL, "Flag. If set, indicates KBP RPF route entry", "FALSE"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed route entry.
 *    It is valid for add/delete/get/update commands.
 * \param [in] unit - unit ID
 * \param [in] route_info - The structure that uniquely describes the accessed entry
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * add
 *          * delete
 *          * destroy
 *          * get
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_route_entry_print(
    int unit,
    bcm_l3_route_t * route_info,
    sh_sand_control_t * sand_control,
    char *action)
{
    int ip_version = 4;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Route entry %s", action);

    PRT_COLUMN_ADD("IP version");
    PRT_COLUMN_ADD("VRF");
    PRT_COLUMN_ADD("IP");
    PRT_COLUMN_ADD("Mask");
    PRT_COLUMN_ADD("FEC destination");
    PRT_COLUMN_ADD("Is raw");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Update the ip_version variable if it is IP6 */
    if (_SHR_IS_FLAG_SET(route_info->l3a_flags, BCM_L3_IP6))
    {
        ip_version = 6;
    }

    PRT_CELL_SET("%d", ip_version);
    PRT_CELL_SET("0x%04X", route_info->l3a_vrf);

    /*
     * Print the ip address and mask values based on the ip version.
     */
    if (ip_version == 4)
    {
        SHR_IF_ERR_EXIT(utilex_pp_ip_addr_numeric_to_string(route_info->l3a_subnet, 1, buffer));
        PRT_CELL_SET("%s", buffer);
        SHR_IF_ERR_EXIT(utilex_pp_ip_addr_numeric_to_string(route_info->l3a_ip_mask, 1, buffer));
    }
    else
    {
        sal_sprintf(buffer, "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
                    route_info->l3a_ip6_net[0], route_info->l3a_ip6_net[1], route_info->l3a_ip6_net[2],
                    route_info->l3a_ip6_net[3], route_info->l3a_ip6_net[4], route_info->l3a_ip6_net[5],
                    route_info->l3a_ip6_net[6], route_info->l3a_ip6_net[7], route_info->l3a_ip6_net[8],
                    route_info->l3a_ip6_net[9], route_info->l3a_ip6_net[10], route_info->l3a_ip6_net[11],
                    route_info->l3a_ip6_net[12], route_info->l3a_ip6_net[13], route_info->l3a_ip6_net[14],
                    route_info->l3a_ip6_net[15]);
        PRT_CELL_SET("%s", buffer);
        sal_sprintf(buffer, "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
                    route_info->l3a_ip6_mask[0], route_info->l3a_ip6_mask[1], route_info->l3a_ip6_mask[2],
                    route_info->l3a_ip6_mask[3], route_info->l3a_ip6_mask[4], route_info->l3a_ip6_mask[5],
                    route_info->l3a_ip6_mask[6], route_info->l3a_ip6_mask[7], route_info->l3a_ip6_mask[8],
                    route_info->l3a_ip6_mask[9], route_info->l3a_ip6_mask[10], route_info->l3a_ip6_mask[11],
                    route_info->l3a_ip6_mask[12], route_info->l3a_ip6_mask[13], route_info->l3a_ip6_mask[14],
                    route_info->l3a_ip6_mask[15]);
    }
    PRT_CELL_SET("%s", buffer);
    PRT_CELL_SET("0x%04X", route_info->l3a_intf);
    PRT_CELL_SET("%s", ((route_info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) ? "YES" : "NO"));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A function that prints some of the accessed multicast entry.
 *    It is valid for add commands.
 * \param [in] unit - unit ID
 * \param [in] data - The structure that uniquely describes the accessed entry
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * add
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_mc_entry_print(
    int unit,
    bcm_ipmc_addr_t * data,
    sh_sand_control_t * sand_control,
    char *action)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Multicast entry %s", action);

    PRT_COLUMN_ADD("FEC destination");
    PRT_COLUMN_ADD("Group (dec)");
    PRT_COLUMN_ADD("Group (hex)");
    PRT_COLUMN_ADD("Port tgid");
    PRT_COLUMN_ADD("Is raw");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("0x%04X", data->l3a_intf);
    PRT_CELL_SET("%d", data->group);
    PRT_CELL_SET("0x%08X", data->group);
    PRT_CELL_SET("%d", data->port_tgid);
    PRT_CELL_SET("%s", ((data->flags & BCM_IPMC_RAW_ENTRY) ? "YES" : "NO"));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an IPv4 or IPv6 route entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_route_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    bcm_ip_t ip_mask;
    bcm_ip6_t ip6_mask;
    int fec;
    int v6_flag;
    int raw_flag, multicast_flag;
    int kbp_fwd, kbp_rpf;
    uint32 vrf;
    int ip_present, ip6_present;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_multicast_t mc_group;
    bcm_vlan_t vid;
    bcm_ip_t mc_ip_addr;
    bcm_ip_t mc_ip_mask;
    bcm_ip6_t mc_ip6_addr;
    bcm_ip6_t mc_ip6_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP4("mask", ip_mask);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_IP6("ip6mask", ip6_mask);
    SH_SAND_GET_INT32("fec", fec);
    SH_SAND_GET_INT32("mc_group", mc_group);
    SH_SAND_GET_UINT32("vid", vid);
    SH_SAND_GET_IP6("mc_ip6", mc_ip6_addr);
    SH_SAND_GET_IP6("mc_ip6mask", mc_ip6_mask);
    SH_SAND_GET_IP4("mc_ip", mc_ip_addr);
    SH_SAND_GET_IP4("mc_mask", mc_ip_mask);

    SH_SAND_GET_BOOL("v6", v6_flag);
    SH_SAND_GET_BOOL("raw", raw_flag);
    SH_SAND_GET_BOOL("multicast", multicast_flag);

    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);

    SH_SAND_GET_BOOL("kbp_fwd", kbp_fwd);
    SH_SAND_GET_BOOL("kbp_rpf", kbp_rpf);

    bcm_l3_route_t_init(&route_entry);
    bcm_ipmc_addr_t_init(&data);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 address together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        route_entry.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
        sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.s_ip6_addr, ip6_address, sizeof(data.s_ip6_addr));
        sal_memcpy(data.s_ip6_mask, ip6_mask, sizeof(data.s_ip6_mask));
        sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
        sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    else
    {
        route_entry.l3a_subnet = ip_address;
        route_entry.l3a_ip_mask = ip_mask;
        data.s_ip_addr = ip_address;
        data.s_ip_mask = ip_mask;
        data.mc_ip_addr = mc_ip_addr;
        data.mc_ip_mask = mc_ip_mask;
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    if (raw_flag == TRUE)
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
        data.flags |= BCM_IPMC_RAW_ENTRY;
    }
    route_entry.l3a_intf = fec;
    route_entry.l3a_vrf = vrf;
    data.l3a_intf = fec;
    data.vrf = vrf;
    if (multicast_flag)
    {
        if (mc_group != 0)
        {
            data.group = mc_group;
            data.l3a_intf = 0;
        }
        data.vid = vid;
        SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &data));
        SHR_IF_ERR_EXIT(bcm_ipmc_find(unit, &data));
        SHR_IF_ERR_EXIT(dnx_mc_entry_print(unit, &data, sand_control, "add"));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));
        SHR_IF_ERR_EXIT(bcm_l3_route_get(unit, &route_entry));
        SHR_IF_ERR_EXIT(dnx_l3_route_entry_print(unit, &route_entry, sand_control, "add"));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes an IPv4 or IPv6 route entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_route_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    bcm_ip_t ip_mask;
    bcm_ip6_t ip6_mask;
    int v6_flag;
    int multicast_flag;
    int kbp_fwd, kbp_rpf;
    uint32 vrf;
    int ip_present, ip6_present;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_vlan_t vid;
    bcm_ip_t mc_ip_addr;
    bcm_ip_t mc_ip_mask;
    bcm_ip6_t mc_ip6_addr;
    bcm_ip6_t mc_ip6_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP4("mask", ip_mask);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_IP6("ip6mask", ip6_mask);
    SH_SAND_GET_UINT32("vid", vid);
    SH_SAND_GET_IP6("mc_ip6", mc_ip6_addr);
    SH_SAND_GET_IP6("mc_ip6mask", mc_ip6_mask);
    SH_SAND_GET_IP4("mc_ip", mc_ip_addr);
    SH_SAND_GET_IP4("mc_mask", mc_ip_mask);

    SH_SAND_GET_BOOL("v6", v6_flag);
    SH_SAND_GET_BOOL("multicast", multicast_flag);

    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);

    SH_SAND_GET_BOOL("kbp_fwd", kbp_fwd);
    SH_SAND_GET_BOOL("kbp_rpf", kbp_rpf);

    bcm_l3_route_t_init(&route_entry);
    bcm_ipmc_addr_t_init(&data);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 address together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        route_entry.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
        sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.s_ip6_addr, ip6_address, sizeof(data.s_ip6_addr));
        sal_memcpy(data.s_ip6_mask, ip6_mask, sizeof(data.s_ip6_mask));
        sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
        sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    else
    {
        route_entry.l3a_subnet = ip_address;
        route_entry.l3a_ip_mask = ip_mask;
        data.s_ip_addr = ip_address;
        data.s_ip_mask = ip_mask;
        data.mc_ip_addr = mc_ip_addr;
        data.mc_ip_mask = mc_ip_mask;
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }

    route_entry.l3a_vrf = vrf;
    data.vrf = vrf;
    if (multicast_flag)
    {
        data.vid = vid;
        SHR_IF_ERR_EXIT(bcm_ipmc_find(unit, &data));
        SHR_IF_ERR_EXIT(bcm_ipmc_remove(unit, &data));
        SHR_IF_ERR_EXIT(dnx_mc_entry_print(unit, &data, sand_control, "delete"));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_l3_route_get(unit, &route_entry));
        SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route_entry));
        SHR_IF_ERR_EXIT(dnx_l3_route_entry_print(unit, &route_entry, sand_control, "delete"));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that updates an IPv4 or IPv6 route entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_route_update_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    bcm_ip_t ip_mask;
    bcm_ip6_t ip6_mask;
    int v6_flag;
    int raw_flag, multicast_flag;
    int kbp_fwd, kbp_rpf;
    uint32 vrf;
    int fec;
    bcm_multicast_t mc_group;
    int ip_present, ip6_present;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_vlan_t vid;
    bcm_ip_t mc_ip_addr;
    bcm_ip_t mc_ip_mask;
    bcm_ip6_t mc_ip6_addr;
    bcm_ip6_t mc_ip6_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP4("mask", ip_mask);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_IP6("ip6mask", ip6_mask);
    SH_SAND_GET_INT32("fec", fec);
    SH_SAND_GET_INT32("mc_group", mc_group);
    SH_SAND_GET_UINT32("vid", vid);
    SH_SAND_GET_IP6("mc_ip6", mc_ip6_addr);
    SH_SAND_GET_IP6("mc_ip6mask", mc_ip6_mask);
    SH_SAND_GET_IP4("mc_ip", mc_ip_addr);
    SH_SAND_GET_IP4("mc_mask", mc_ip_mask);

    SH_SAND_GET_BOOL("v6", v6_flag);
    SH_SAND_GET_BOOL("raw", raw_flag);
    SH_SAND_GET_BOOL("multicast", multicast_flag);

    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);

    SH_SAND_GET_BOOL("kbp_fwd", kbp_fwd);
    SH_SAND_GET_BOOL("kbp_rpf", kbp_rpf);

    bcm_l3_route_t_init(&route_entry);
    bcm_ipmc_addr_t_init(&data);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 address together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        route_entry.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
        sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.s_ip6_addr, ip6_address, sizeof(data.s_ip6_addr));
        sal_memcpy(data.s_ip6_mask, ip6_mask, sizeof(data.s_ip6_mask));
        sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
        sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    else
    {
        route_entry.l3a_subnet = ip_address;
        route_entry.l3a_ip_mask = ip_mask;
        data.s_ip_addr = ip_address;
        data.s_ip_mask = ip_mask;
        data.mc_ip_addr = mc_ip_addr;
        data.mc_ip_mask = mc_ip_mask;
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    if (raw_flag == TRUE)
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
        data.flags |= BCM_IPMC_RAW_ENTRY;
    }
    route_entry.l3a_flags |= BCM_L3_REPLACE;
    route_entry.l3a_intf = fec;
    route_entry.l3a_vrf = vrf;
    data.flags |= BCM_IPMC_REPLACE;
    data.l3a_intf = fec;
    data.vrf = vrf;

    if (multicast_flag)
    {
        if (mc_group != 0)
        {
            data.group = mc_group;
            data.l3a_intf = 0;
        }
        data.vid = vid;
        SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &data));
        SHR_IF_ERR_EXIT(bcm_ipmc_find(unit, &data));
        SHR_IF_ERR_EXIT(dnx_mc_entry_print(unit, &data, sand_control, "update"));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route_entry));
        SHR_IF_ERR_EXIT(bcm_l3_route_get(unit, &route_entry));
        SHR_IF_ERR_EXIT(dnx_l3_route_entry_print(unit, &route_entry, sand_control, "update"));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an IPv4 or IPv6 route entry.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_route_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_ip_t ip_address;
    bcm_ip6_t ip6_address;
    bcm_ip_t ip_mask;
    bcm_ip6_t ip6_mask;
    int v6_flag;
    int raw_flag, multicast_flag;
    int kbp_fwd, kbp_rpf;
    uint32 vrf;
    int ip_present, ip6_present;
    bcm_l3_route_t route_entry;
    bcm_ipmc_addr_t data;
    bcm_vlan_t vid;
    bcm_ip_t mc_ip_addr;
    bcm_ip_t mc_ip_mask;
    bcm_ip6_t mc_ip6_addr;
    bcm_ip6_t mc_ip6_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("vrf", vrf);
    SH_SAND_GET_IP4("IP", ip_address);
    SH_SAND_GET_IP4("mask", ip_mask);
    SH_SAND_GET_IP6("IP6", ip6_address);
    SH_SAND_GET_IP6("ip6mask", ip6_mask);
    SH_SAND_GET_UINT32("vid", vid);
    SH_SAND_GET_IP6("mc_ip6", mc_ip6_addr);
    SH_SAND_GET_IP6("mc_ip6mask", mc_ip6_mask);
    SH_SAND_GET_IP4("mc_ip", mc_ip_addr);
    SH_SAND_GET_IP4("mc_mask", mc_ip_mask);

    SH_SAND_GET_BOOL("v6", v6_flag);
    SH_SAND_GET_BOOL("raw", raw_flag);
    SH_SAND_GET_BOOL("multicast", multicast_flag);

    SH_SAND_IS_PRESENT("IP", ip_present);
    SH_SAND_IS_PRESENT("IP6", ip6_present);

    SH_SAND_GET_BOOL("kbp_fwd", kbp_fwd);
    SH_SAND_GET_BOOL("kbp_rpf", kbp_rpf);

    bcm_l3_route_t_init(&route_entry);
    bcm_ipmc_addr_t_init(&data);
    /*
     * If both parameters ip and ip6 are provided, return an error.
     * Otherwise set the IP6 flag if it's IPv6 address and assign the values to the appropriate field of the structure.
     */
    if (ip_present && ip6_present)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set an IPv4 address and an IPv6 address together.\n");
    }
    else if ((ip6_present && !ip_present) || (v6_flag == TRUE))
    {
        route_entry.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route_entry.l3a_ip6_net, ip6_address, sizeof(bcm_ip6_t));
        sal_memcpy(route_entry.l3a_ip6_mask, ip6_mask, sizeof(bcm_ip6_t));
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.s_ip6_addr, ip6_address, sizeof(data.s_ip6_addr));
        sal_memcpy(data.s_ip6_mask, ip6_mask, sizeof(data.s_ip6_mask));
        sal_memcpy(data.mc_ip6_addr, mc_ip6_addr, sizeof(data.mc_ip6_addr));
        sal_memcpy(data.mc_ip6_mask, mc_ip6_mask, sizeof(data.mc_ip6_mask));
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    else
    {
        route_entry.l3a_subnet = ip_address;
        route_entry.l3a_ip_mask = ip_mask;
        data.s_ip_addr = ip_address;
        data.s_ip_mask = ip_mask;
        data.mc_ip_addr = mc_ip_addr;
        data.mc_ip_mask = mc_ip_mask;
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            route_entry.l3a_flags2 |= (kbp_fwd == TRUE) ? BCM_L3_FLAGS2_FWD_ONLY :
                (kbp_rpf == TRUE) ? BCM_L3_FLAGS2_RPF_ONLY : 0;
        }
    }
    if (raw_flag == TRUE)
    {
        route_entry.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
        data.flags |= BCM_IPMC_RAW_ENTRY;
    }
    route_entry.l3a_vrf = vrf;
    data.vrf = vrf;

    if (multicast_flag)
    {
        data.vid = vid;
        SHR_IF_ERR_EXIT(bcm_ipmc_find(unit, &data));
        SHR_IF_ERR_EXIT(dnx_mc_entry_print(unit, &data, sand_control, "get"));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_l3_route_get(unit, &route_entry));
        SHR_IF_ERR_EXIT(dnx_l3_route_entry_print(unit, &route_entry, sand_control, "get"));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_route_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"add", sh_dnx_l3_route_add_cmd, NULL, dnx_l3_route_add_options, &dnx_l3_route_add_man}
    ,
    {"get", sh_dnx_l3_route_get_cmd, NULL, dnx_l3_route_get_options, &dnx_l3_route_get_man}
    ,
    {"update", sh_dnx_l3_route_update_cmd, NULL, dnx_l3_route_add_options, &dnx_l3_route_update_man}
    ,
    {"delete", sh_dnx_l3_route_delete_cmd, NULL, dnx_l3_route_delete_options, &dnx_l3_route_delete_man}
    ,
    {NULL}
};
