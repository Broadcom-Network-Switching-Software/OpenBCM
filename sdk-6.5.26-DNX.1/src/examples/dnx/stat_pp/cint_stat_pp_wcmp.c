/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_stat_pp_wcmp.c Purpose: Statistics examples for SDK ingress APIs using an IPV4/6 WCMP application.
 */

/*
 * File: cint_stat_pp_wcmp.c
 * Purpose: Example of ingress statistics with WCMP
 *
 * Configure statistics on WCMP in fwd
 *
 * The cint includes:
 * - Main function to configure routing with statistics enabled along the way
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with ip route and wcmp required configuration
 *
 */

struct cint_stat_pp_wcmp_info_s
{
    int in_port;                                        /** incoming port */
    int out_port;                                       /** outgoing port */
    int intf_in;                                        /** Incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address;                      /** mac for in RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                         /** mac for out RIF */
    int encap_id;                                       /** encap ID */
    bcm_ip_t ipv4_addr;                               /** IPv4 address */
    bcm_ip6_t ipv6_addr;                                /** IPv6 address */
    int vrf;                                            /** VRF */
    bcm_if_t fec_id;                                    /** FEC id */
    bcm_if_t wcmp_id;                                   /** WCMP id */
};

cint_stat_pp_wcmp_info_s cint_stat_pp_wcmp_info =
{
    /*
     * ports : in_port | out_port
     */
    200, 201,
    /*
     * intf_in | intf_out
     */
    15, 100,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * encap_id
     */
    0x1384,
    /*
     * ipv4_addr
     */
    0x7fffff02 /** 127.255.255.2 */,
    /*
     * ipv6_addr
     */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x12 } /** 0000:0000:0000:0000:1234:0000:0000:FF12 */,
    /*
     * vrf
     */
    1,
    /*
     * fec_id (set on run time)
     */
    0,
    /*
     * wcmp_id (set on run time)
     */
    0
};

cint_stat_pp_config_info_s l3_wcmp_statistics_config =
{
        /**database_id */
        8,
        /**crps_engine_id */
        8,
        /**pp_profile */
        8,
        /**counter_command */
        8,
        /**crps_start */
        0,
        /**crps_end */
        800,
        /**type_id*/
        0,
        /**stat_id*/
        2,
        /**expansion_flag*/
        5
};

/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configures routing based on dnx_basic_example
 * WCMP group with statistics
 * IPv4 and IPv6 route entries to forward using WCMP
 */
int
dnx_stat_pp_wcmp_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    int encoded_fec;
    bcm_l3_route_t route;
    bcm_ip6_t ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    char *proc_name;

    proc_name = "dnx_stat_pp_wcmp_example";

    int separate_fwd_rpf_dbs = *(dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL));
    uint32 l3_flags2_fwd_only = separate_fwd_rpf_dbs ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    uint32 l3_flags2_rpf_only = separate_fwd_rpf_dbs ? BCM_L3_FLAGS2_RPF_ONLY : 0;

    cint_stat_pp_wcmp_info.in_port = in_port;
    cint_stat_pp_wcmp_info.out_port = out_port;

    /** Configure statistics for l3 wcmp */
    rv = cint_stat_pp_config_main(unit, l3_wcmp_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for l3_wcmp_statistics_config\n");
        return rv;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &cint_stat_pp_wcmp_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, get_first_fec_in_range_which_not_in_ecmp_range hierarchy 2\n", proc_name);
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_stat_pp_wcmp_info.in_port, cint_stat_pp_wcmp_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, cint_stat_pp_wcmp_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n", proc_name, cint_stat_pp_wcmp_info.out_port);
        return rv;
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_stat_pp_wcmp_info.intf_in, cint_stat_pp_wcmp_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n", proc_name, cint_stat_pp_wcmp_info.intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_stat_pp_wcmp_info.intf_out, cint_stat_pp_wcmp_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out %d\n", proc_name, cint_stat_pp_wcmp_info.intf_out);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = cint_stat_pp_wcmp_info.vrf;
    ingress_rif.intf_id = cint_stat_pp_wcmp_info.intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set\n", proc_name);
        return rv;
    }
    /*
     * 5. Create ARP and set its properties
     */
    rv = create_arp_plus_ac_type(unit, 0, cint_stat_pp_wcmp_info.arp_next_hop_mac,
                                 &cint_stat_pp_wcmp_info.encap_id, cint_stat_pp_wcmp_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_arp_plus_ac_type\n", proc_name);
        return rv;
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (BCM_GPORT_IS_SET(cint_stat_pp_wcmp_info.out_port))
    {
       gport = cint_stat_pp_wcmp_info.out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, cint_stat_pp_wcmp_info.out_port);
    }
    rv = l3__egress_only_fec__create_inner(unit, cint_stat_pp_wcmp_info.fec_id, cint_stat_pp_wcmp_info.intf_out,
                                           cint_stat_pp_wcmp_info.encap_id, gport, BCM_L3_2ND_HIERARCHY, l3_flags2_fwd_only, 0, 0, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n", proc_name, cint_stat_pp_wcmp_info.fec_id, encoded_fec);

    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, cint_stat_pp_wcmp_info.fec_id);
    if (BCM_GPORT_IS_SET(cint_stat_pp_wcmp_info.out_port))
    {
       gport = cint_stat_pp_wcmp_info.out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, cint_stat_pp_wcmp_info.out_port);
    }

    /*
     * 7. Create WCMP group with a single FEC and statistics
     */
    bcm_l3_egress_ecmp_t wcmp;
    bcm_l3_ecmp_member_t intf_array[1];

    bcm_l3_ecmp_member_t_init(&intf_array[0]);
    intf_array[0].egress_if = cint_stat_pp_wcmp_info.fec_id;
    intf_array[0].weight = 1000;

    bcm_l3_egress_ecmp_t_init(&wcmp);
    wcmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
    wcmp.stat_id = l3_wcmp_statistics_config.stat_id;
    wcmp.stat_pp_profile = l3_wcmp_statistics_config.pp_profile;
    rv = bcm_l3_ecmp_create(unit, 0, &wcmp, 1, intf_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ecmp_create\n");
    }
    cint_stat_pp_wcmp_info.wcmp_id = wcmp.ecmp_intf;

    /*
     * 8. Add route entries
     */

    /** Add ipv4 route entry */
    bcm_l3_route_t_init(route);
    route.l3a_subnet = cint_stat_pp_wcmp_info.ipv4_addr;
    route.l3a_ip_mask = 0xFFFFFFFF;
    route.l3a_vrf = cint_stat_pp_wcmp_info.vrf;
    route.l3a_intf = cint_stat_pp_wcmp_info.wcmp_id;
    route.l3a_flags2 = l3_flags2_fwd_only;
    rv = bcm_l3_route_add(unit, route);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_route_add IPv4\n");
    }

    /** Add ipv6 route entry */
    bcm_l3_route_t_init(route);
    route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(route.l3a_ip6_net, cint_stat_pp_wcmp_info.ipv6_addr, 16);
    sal_memcpy(route.l3a_ip6_mask, ipv6_mask, 16);
    route.l3a_subnet = cint_stat_pp_wcmp_info.ipv4_addr;
    route.l3a_ip_mask = 0xFFFFFFFF;
    route.l3a_vrf = cint_stat_pp_wcmp_info.vrf;
    route.l3a_intf = cint_stat_pp_wcmp_info.wcmp_id;
    route.l3a_flags2 = l3_flags2_fwd_only;
    rv = bcm_l3_route_add(unit, route);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_route_add IPv4\n");
    }

    printf("%s(): Exit\r\n", proc_name);
    return rv;
}
