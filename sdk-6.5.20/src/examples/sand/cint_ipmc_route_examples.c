/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /*
 * Add an IPv6 raw entry. Entry value is 'fec'
 * - mc_ip - the MC group DIP
 * - src_ip - the SIP
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - mc_group - the MC group
 *
 * Note:
 *    Raw access is forced via the 'BCM_IPMC_RAW_ENTRY' flag.
 */
int
add_ipv6_ipmc_raw (
    int unit,
    bcm_ip6_t mc_ip,
    bcm_ip6_t src_ip,
    bcm_vrf_t eth_rif_id,
    bcm_multicast_t mc_group,
    int fec,
    int vrf)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    int i = 0;
    int flags = BCM_IPMC_IP6;

    if (is_device_or_above(unit, JERICHO2))
    {
        if ((*dnxc_data_get(unit, "elk", "application", "ipv6", NULL)))
        {
            /* In Jericho 2 with KBP only TCAM mode is supported for IPv6 MC */
            flags |= BCM_IPMC_TCAM;
        }
    }
    else
    {
        flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    bcm_ipmc_addr_t_init(&data);
    for (i = 0; i < 16; i++) {
        data.s_ip6_addr[i] = src_ip[i];
        data.mc_ip6_addr[i] = mc_ip[i];
        data.mc_ip6_mask[i] = 0xFF;
    }

    data.flags = flags | BCM_IPMC_RAW_ENTRY;
    data.vid = 0;
    data.vrf = vrf;
    data.l3a_intf = lpm_payload;
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

/* Find an IPv6 MC entry
*/
int
find_ipv6_ipmc (
    int unit,
    bcm_ip6_t mc_ip,
    bcm_ip6_t src_ip,
    int vlan,
    bcm_multicast_t mc_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    int i = 0;
    int flags = BCM_IPMC_IP6;

    if (is_device_or_above(unit, JERICHO2))
    {
        if ((*dnxc_data_get(unit, "elk", "application", "ipv6", NULL)))
        {
            /* In Jericho 2 with KBP only TCAM mode is supported for IPv6 MC */
            flags |= BCM_IPMC_TCAM;
        }
    }
    else
    {
        flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    bcm_ipmc_addr_t_init(&data);
    for (i = 0; i < 16; i++) {
        data.s_ip6_addr[i] = src_ip[i];
        data.mc_ip6_addr[i] = mc_ip[i];
        data.mc_ip6_mask[i] = 0xFF;
    }

    data.flags = flags;
    data.vid = vlan;
    data.group = mc_id;

    rv = bcm_ipmc_find(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, in ipmc find, mc_ip $mc_ip src_ip $src_ip\n");
        return rv;
    }

    return rv;
}

/*
 * Example of a simple-router IPMC scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ipmc_route_basic.c
 * cint src/examples/sand/cint_ipmc_route_examples.c
 * cint
 * ipmc_to_trap_example(0,0xCC,200);
 * exit;
 *
 * ETH-RIF packet to be sent to trap:
 * tx 1 psrc = 200 data = 01005e020203000007000100810000cc0800450000350000000080009742c0800101e0020203000102030405
 */

/*
 * Configure interfaces, user-defined trap, routes and hosts for MC forwarding.
 * The function creates IPMC entries that result to a trap gport.
 * It is expected that the packet will arrive on port in_port.
 * It is expected that if the DIP matches an entry that results to a port(trap) it will be dropped.
 *
 * It is expected that the input packet will have
 *      * DA = 01:00:5E:02:02:02
 *      * SA = 00:00:07:00:01:00
 *      * VID = rif (input)
 *      * DIP = 224.2.2.2 - 224.2.2.3
 *      * SIP = 192.128.1.1
 * IPMC entries with DIP 224.2.2.2 is saved in LEM.
 * IPMC entries with DIP 224.2.2.3 is saved in TCAM
 */
int
ipmc_to_trap_example(
    int unit,
    int vid_in,
    int in_port,
    int trap_id_fec)
{
    int rv = BCM_E_NONE;
    int eth_rif_id = vid_in;
    int idx;
    int nof_entries = 2;
    bcm_gport_t trap_gport;
    bcm_rx_trap_config_t config;
    uint32 full_mask = 0x0;
    uint32 part_mask = 0xFFFF0000;
    uint32 no_mask = 0xFFFFFFFF;
    cint_ipmc_info.intf_in = vid_in;
    cint_ipmc_info.in_port = in_port;
    l3_ipmc_ipv4 ipmc_entry[2];

    /** LEM entry */
    l3_ipmc_ipv4_init(&ipmc_entry[0], cint_ipmc_info.vrf, cint_ipmc_info.mc_host_dip, no_mask,
                      cint_ipmc_info.host_sip, full_mask, cint_ipmc_info.intf_in, 0);

    /** TCAM entry */
    l3_ipmc_ipv4_init(&ipmc_entry[1], cint_ipmc_info.vrf, cint_ipmc_info.mc_host_dip + 1, no_mask,
                      cint_ipmc_info.host_sip, part_mask, cint_ipmc_info.intf_in, 0);
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = eth_rif_id;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * 4. Set user defined traps and define gport to the trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    rv = bcm_rx_trap_set(unit, trap_id_fec, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_set\n");
        return rv;
    }

    /*
     * Set maximum Trap strength to the Trap ID when creating the gport.
     */
    BCM_GPORT_TRAP_SET(trap_gport, trap_id_fec, 15, 0);
    printf("trap gport = %04x, trap_id = %04x\n", trap_gport, trap_id_fec);

    /*
     * 5. Add IPv4 IPMC entries - iterate over the different tables - EM, TCAM, KAPS
     */
    for (idx = 0; idx < nof_entries; idx++)
    {
        rv = add_ipv4_ipmc_to_port(unit, ipmc_entry[idx].dip, no_mask, ipmc_entry[idx].sip, 0,
                                   ipmc_entry[idx].rif, ipmc_entry[idx].vrf, trap_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv4_ipmc_to_port, idx = %d \n", idx);
            return rv;
        }
    }

    return rv;
}

/*
 * Test Scenario
 *     Example of a simple-router IPMC scenario (1. trap to cpu 2. drop)
 *
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_examples.c
 * cint
 * ipv4_mc_example_trap(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006b85bc0800104e0e0e0013344556600a1a2a300b1b2b3500f01f57e4e0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e00004000400606d178fffa0ee0e0e0023344556600a1a2a300b1b2b3500f01f5ccc30000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x002e000040003f0607d178fffa0ee0e0e0023344556600a1a2a300b1b2b3500f01f5ccc30000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

int print_level = 2;
print_level = 2;

/* Add an IPv4 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int
add_ip4mc_entry_with_egress_itf(
    int unit,
    bcm_ip_t mc_ip,
    bcm_ip_t src_ip,
    int vlan,
    int vrf,
    int egress_itf)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    int flags = 0;

    if (!is_device_or_above(unit, JERICHO2))
    {
        flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    /* Init the ipmc_addr structure and fill the requiered fields.*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    data.s_ip_addr = src_ip;
    data.vid = vlan;
    data.vrf = vrf;
    data.flags = flags;
    data.l3a_intf = egress_itf;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
        return rv;
    }

    if(print_level >= 1) {
        printf("G=0x%08x S=0x%08x V=0x%08x -->  0x%08x\n", mc_ip, src_ip, vlan, egress_itf);
    }

    return rv;
}

int
ipv4_mc_example_trap(
    int unit,
    int in_port,
    int out_port)
{
    bcm_error_t rv = BCM_E_NONE;
    int ingress_intf;

    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    /* compatible MAC 01:00:5E:60:e0:01 */
    bcm_ip_t src_ip = 0x0; /* masked */
    int ipmc_index = 6000;
    int dest_local_port_id = 13;
    int source_local_port_id = 13;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

    int egr_flags = 0;
    bcm_multicast_t mc_id;
    bcm_gport_t mc_gport_drop; /*This mc id traps to BCM_GPORT_BLACK_HOLE*/
    int nof_entries = 6;
    int idx;

    bcm_l3_egress_t l3eg;
    int fec[10] = {0x0};
    int eg_itf_index = 0;

    int encap_id[10]={0x0};
    int vrf = 0;
    int vlan;

    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    int ing_intf_l;
    bcm_gport_t mc_gport_2_cpu; /*This mc id traps to BCM_GPORT_LOCAL_CPU*/

    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    int ing_intf_r;

    int trap_id_handle_drop, trap_id_handle_cpu;
    bcm_rx_trap_config_t config_trap_drop, config_trap_cpu;
    int trap_strength;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);


    vrf = vlan_l;

    /* 1. configure two traps: dest port of one trap is drop, dest of another trap is cpu */
    bcm_rx_trap_type_create(unit,0/*flags*/,bcmRxTrapUserDefine,&trap_id_handle_drop);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_type_create - bcmRxTrapUserDefine\n");
        return rv;
    }
    bcm_rx_trap_type_create(unit,0/*flags*/,bcmRxTrapUserDefine,&trap_id_handle_cpu);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_type_create - bcmRxTrapUserDefine\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&config_trap_drop);
    config_trap_drop.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config_trap_drop.trap_strength = 0; /*FOR USER DEFINE set as part of destination */
    config_trap_drop.dest_port = BCM_GPORT_BLACK_HOLE;
    rv = bcm_rx_trap_set(unit,trap_id_handle_drop,&config_trap_drop);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");
    }

    bcm_rx_trap_config_t_init(&config_trap_cpu);
    config_trap_cpu.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config_trap_cpu.trap_strength = 0; /*FOR USER DEFINE set as part of destination */
    BCM_GPORT_LOCAL_SET(config_trap_cpu.dest_port, 0);
    rv = bcm_rx_trap_set(unit,trap_id_handle_cpu,&config_trap_cpu);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");

    }

    /* Associate the traps with the gports */
    trap_strength = (is_device_or_above(unit, JERICHO2)) ? 15 : 7;
    BCM_GPORT_TRAP_SET(mc_gport_drop, trap_id_handle_drop, trap_strength, 0);
    BCM_GPORT_TRAP_SET(mc_gport_2_cpu, trap_id_handle_cpu, trap_strength, 0);


    /* 2. Set In-Port to Eth-RIF */
    rv = in_port_intf_set(unit, in_port, vlan_l);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /* 3. Set Out-Port default properties */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    /* 4. Create IN-RIF and set its properties */
    rv = intf_eth_rif_create(unit, vlan_l, mac_address_l);
    ing_intf_l = vlan_l;
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, vlan_r, mac_address_r);
    ing_intf_r = vlan_r;
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /* 5. Set ingress interface */
    ingr_itf.intf_id = ing_intf_l;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 6. create two egress objects with different traps
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * For Jr2 skip creating FECs with trap gport destinations as it is not supported.
         * Instead set the number of IPMC entries that are going to be created.
         */
        eg_itf_index = 2;
    }
    else
    {
        /** Create two FECs with different destinations - gport trap to CPU and gport trap drop */
        bcm_l3_egress_t_init(&l3eg);
        l3eg.flags = 0;
        l3eg.intf = ing_intf_r;
        l3eg.port = mc_gport_drop;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rv;
        }
        ++eg_itf_index;


        l3eg.flags = BCM_L3_MC_RPF_SIP_BASE;
        l3eg.intf = ing_intf_r;
        l3eg.port = mc_gport_2_cpu;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rv;
        }
        ++eg_itf_index;
    }

    /* 7. Create the IP MC entry */
    vlan = (is_device_or_above(unit, JERICHO2)) ? vlan_l: 0;
    for(idx = 0; idx < eg_itf_index; ++idx) {
        if (is_device_or_above(unit, JERICHO2))  {
            rv = add_ipv4_ipmc_to_port(unit, mc_ip, 0xffffffff, src_ip, 0x0, vlan, vrf, ((idx==0)?mc_gport_drop:mc_gport_2_cpu));
        } else {
            rv = add_ip4mc_entry_with_egress_itf(unit, mc_ip, src_ip, vlan, vrf, fec[idx]);
        }
        if (rv != BCM_E_NONE)
        {
            return rv;
        }
        mc_ip++;

    }

    return rv;
}

/*
 * Test Scenario 
 *     Example of a simple-router IPMC scenario with different prefix
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_examples.c
 * cint
 * ipmc_prefixes_test(0,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x000c000200000000070001008100006408004508002e00004000400657dac080fefeef1234563344556600a1a2a300b1b2b3500f01f51dcd0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000001000c000200008100006408004508002e000040003f0658dac080fefeef1234563344556600a1a2a300b1b2b3500f01f51dcd0000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=200 data=0x000c000200000000070001008100006408004508002e0000400040060e000c08fefeefab32103344556600a1a2a300b1b2b3500f01f5d3f20000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x00000002000c000200008100006408004508002e000040003f060f000c08fefeefab32103344556600a1a2a300b1b2b3500f01f5d3f20000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */


int
ipmc_prefixes_test(
    int unit,
    int inP,
    int outP)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_l3_intf_t l3Intf;
    bcm_mac_t mac_addr = {0x00,0x0c,0x00,0x02,0x00,0x00};
    int vlan =100;
    int ipmcGroup[2] = {0x1770, 0x1771};
    int intf_out = 300;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;


    bcm_l3_intf_t       l3Intf_for_egress[2];
    bcm_l3_egress_t     egress_intf[2];
    bcm_if_t            eg_int[2];
    bcm_mac_t           next_hop_mac[2] = {{0xa0,0x00,0x00,0x00,0x00,0x01},{0xb0,0x00,0x00,0x00,0x00,0x02}};

    bcm_ipmc_addr_t data;
    bcm_ip_t mc_ip[2]           = {0xef123456, 0xefabcdef};
    bcm_ip_t s_ip[2]            = {0xc0800101, 0x00000000};
    bcm_ip_t mc_ip_mask[2]      = {0xffffffff, 0xffff0000};
    bcm_ip_t s_ip_mask[2]       = {0xffff0000, 0x00000000};

    bcm_if_t l3egid_null[2] = {0x2000, 0x2002};
    int fecIds[2] = { 40961, 40962 };
    bcm_gport_t gport;
    bcm_multicast_replication_t replications;
    int flags;

    rv = in_port_intf_set(unit, inP, vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set vlan\n");
        return rv;
    }

    /* Interface1 for ingress*/
    bcm_l3_intf_t_init(&l3Intf);
    sal_memcpy(l3Intf.l3a_mac_addr, mac_addr, 6);
    l3Intf.l3a_vid = vlan;
    l3Intf.l3a_vrf = vlan;
    rv = bcm_l3_intf_create(unit, &l3Intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    for (i = 0; i < 2; i++) 
    {
        bcm_l3_intf_t_init(&l3Intf_for_egress[i]);
        sal_memcpy(l3Intf_for_egress[i].l3a_mac_addr, mac_addr, 6);
        l3Intf_for_egress[i].l3a_vid = vlan+i+1;
        rv = bcm_l3_intf_create(unit, &l3Intf_for_egress[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create l3Intf_for_egress %d\n",i);
            return rv;
        }

        bcm_l3_egress_t_init(&egress_intf[i]);
        flags = BCM_L3_EGRESS_ONLY;
        sal_memcpy(egress_intf[i].mac_addr, next_hop_mac[i], 6);
        egress_intf[i].encap_id = l3egid_null[i];
        egress_intf[i].vlan = vlan+i+1;
        rv = bcm_l3_egress_create(unit,flags,&egress_intf[i],&l3egid_null[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_create %d\n",i);
            return rv;
        }

        flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID) :
            (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
        rv = bcm_multicast_create(unit, flags, &ipmcGroup[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_create %d\n",i);
            return rv;
        }

        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP: BCM_MULTICAST_INGRESS;
        bcm_multicast_replication_t_init(&replications);
        replications.encap1 = egress_intf[i].encap_id & 0x7ffff;
        replications.port = outP;
        rv = bcm_multicast_set(unit, ipmcGroup[i], flags, 1, &replications);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_set %d\n",i);
            return rv;
        }

        bcm_ipmc_addr_t_init(&data);
        data.vrf = vlan;
        data.mc_ip_addr = mc_ip[i];
        data.mc_ip_mask = mc_ip_mask[i];
        data.s_ip_addr = s_ip[i];
        data.s_ip_mask = s_ip_mask[i];
        data.flags = 0;
        data.group = ipmcGroup[i];
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2 */
            data.flags |= BCM_IPMC_TCAM;
        }

        /* Creates the entry */
        rv = bcm_ipmc_add(unit,&data);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_ipmc_add %d\n",i);
            return rv;
        }
    }
    return rv;
}

/*
 * In this example, we send a packet with MC IP 224.224.224.1, dedicated mymac and vlan 2, through the iport.
 *
 * We then expect the packet to multicast and exit on four different rifs:
 *      - oport + outrif 2 - will fallback to bridge and be filtered because of same interface filter
 *      - (oport + 1) + outrif 3 (vlan 3)
 *      - (oport + 2) + ourtif 4 (vlan 4)
 *      - (oport + 3) + ourtif 5 (vlan 5)
 *      Expected DA for these packets is the mac_l3_ingress: 00:00:00:00:ab:1d
 */
int
run_with_ports_ipv4_mc_example(
    int unit,
    int iport,
    int oport)
{
    uint32 rv = BCM_E_NONE;
    bcm_gport_t dst_gport;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
    int vlan = 2;
    int vrf = 2;
    int ipmc_index = 6000;
    int dest_local_port_id = iport;
    int source_local_port_id = oport;
    int nof_dest_ports = 4, port_ndx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};
    l3_ingress_intf ingress_itf;

    bcm_multicast_t mc_id = create_ip_mc_group(unit,
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP),
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE);

    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        rv = in_port_intf_set(unit, dest_local_port_id + port_ndx, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set vlan\n");
            return rv;
        }
        rv = out_port_set(unit, (dest_local_port_id+port_ndx));
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, out_port_intf_set intf_out\n");
            return rv;
        }

        rv = intf_eth_rif_create(unit, (vlan+port_ndx), mac_l3_ingress);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", (vlan+port_ndx), rv);
            return rv;
        }

        ingress_itf.vrf = vrf + port_ndx;
        ingress_itf.intf_id = vlan + port_ndx;
        rv = intf_ingress_rif_set(unit, &ingress_itf);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, vlan, vrf, rv);
            return rv;
        }

        if (lpm_raw_data_enabled) {
            rv = add_ipv4_ipmc_raw(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vlan+port_ndx), (vrf+port_ndx), 0, lpm_payload);
        } else {
            rv = add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vlan+port_ndx), (vrf+port_ndx), mc_id, 0, 0);
        }
        if (rv != BCM_E_NONE){
            return rv;
        }
    }

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        BCM_GPORT_LOCAL_SET(dst_gport, oport);
        if (is_device_or_above(unit, JERICHO2)){
            rv = dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport);
        } else {
            rv = dpp_kaps_raw_data_redirect(unit, lpm_payload, dst_gport);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    return rv;
}

/* Main IPv4 MC Example with sip/dip prefix parameter*/
int run_with_prefix_ipv4_mc_example(
    int unit,
    int iport,
    int oport,
    int sip_prefix_len,
    int dip_prefix_len,
    int nof_dest_ports,
    int ipmc_index,
    int use_inrif)

{
    uint32 rv = BCM_E_NONE;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
    bcm_ip_t s_ip_mask = 0;
    bcm_ip_t d_ip_mask = 0;
    int vlan = 2;
    int vrf = 2;
    int dest_local_port_id = iport;
    int source_local_port_id = oport;
    int port_ndx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};
    l3_ingress_intf ingress_itf;

    s_ip_mask = bcm_ip_mask_create(sip_prefix_len);
    d_ip_mask = bcm_ip_mask_create(dip_prefix_len);
    rv = bcm_vlan_gport_add(unit, vlan, iport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    bcm_multicast_destroy(unit,ipmc_index);
    bcm_multicast_t mc_id = create_ip_mc_group(unit,
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP),
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE);
    /* Create ingress intf, for ethernet termination. */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = vlan__open_vlan_per_mc(unit, vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan, unit);
        }
    }

    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        rv = out_port_set(unit, (dest_local_port_id+port_ndx));
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, out_port_intf_set intf_out\n");
            return rv;
        }

        int temp_vlan = vlan + port_ndx;
        rv = intf_eth_rif_create(unit, temp_vlan, mac_l3_ingress);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", temp_vlan, rv);
            return rv;
        }

        ingress_itf.vrf = vrf + port_ndx;
        ingress_itf.intf_id = temp_vlan;
        rv = intf_ingress_rif_set(unit, &ingress_itf);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", temp_vlan, vrf, rv);
            return rv;
        }
        /* Create the IP MC entry with s_ip_mask */
        if (is_device_or_above(unit, JERICHO2)) {
            if (use_inrif && sip_prefix_len > 0 && sip_prefix_len < 32) {
                /*Write this entry to TCAM with vlan != 0*/
                rv = add_ipv4_ipmc(unit, mc_ip, d_ip_mask, src_ip, s_ip_mask, temp_vlan, (vrf+port_ndx), mc_id, 0, BCM_IPMC_TCAM);
            }else {
                rv = add_ipv4_ipmc(unit, mc_ip, d_ip_mask, src_ip, s_ip_mask, 0, (vrf+port_ndx), mc_id, 0, 0);
            }
        }else {
            rv = add_ipv4_ipmc(unit, mc_ip, d_ip_mask, src_ip, s_ip_mask, 0, (vrf+port_ndx), mc_id, 0, 0);
        }
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS){
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Basic MC routing functionality is tested (while loading LPM * table with a 'raw' value)
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_scenarios.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_examples.c
 * cint
 * lpm_raw_data_enabled=1;
 * lpm_payload=0x12345;
 * run_with_ports_ipv6_mc_example(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x3333000000010000000000018100000286dd608000100014060a12cd0000000000000000000000000001ff0000000000000000000000000000013344556600a1a2a300b1b2b3500001f5bccc0000
 */
 /* Received packets on unit 0 should be:
  * Source port: 0, Destination port: 0
  * Data: 0x01b800640000000000a80c048416048416000600001040520e820090120005608000100014060912cd0000000000000000000000000001ff00000000
 */
int
run_with_ports_ipv6_mc_example (
    int unit,
    int iport,
    int oport)
{
    bcm_error_t rv = BCM_E_NONE;
    int ingress_intf;
    int nof_dest_ports = 3;

    /* Set parameters: User can set different parameters. */
    bcm_ip6_t s_ip6 = {18, 205, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    bcm_ip6_t mc_ip6 = {255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    int ipmc_index = 6001;
    int dest_local_port_id = oport;
    int vlan = 1, port_ndx;
    int source_local_port_id = iport;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

    int vrf = 2;
    bcm_gport_t dst_gport;
    l3_ingress_intf ingress_itf;

    /* Create the IP MC Group */
    bcm_multicast_destroy(unit,ipmc_index);
    bcm_multicast_t mc_id = create_ip_mc_group(unit,
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP),
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE);

    /* Add member VLAN to each port in the multicast group */
    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        rv = bcm_vlan_gport_add(unit, vlan + port_ndx, dest_local_port_id+port_ndx, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", dest_local_port_id+port_ndx, vlan + port_ndx);
            return rv;
        }
    }

    /* Create ingress intf, for ethernet termination. */
    rv = vlan__open_vlan_per_mc(unit, vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, vlan, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, vlan);
        return rv;
    }

    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx)
    {
        rv = intf_eth_rif_create(unit, (vlan+port_ndx), mac_l3_ingress);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", (vlan+port_ndx), rv);
            return rv;
        }

        ingress_itf.vrf = vrf + port_ndx;
        ingress_itf.intf_id = vlan + port_ndx;
        rv = intf_ingress_rif_set(unit, &ingress_itf);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", vlan, vrf, rv);
            return rv;
        }

        /* Create the IPv6 MC Entry */
        rv = add_ipv6_ipmc_raw(unit, mc_ip6, s_ip6, vlan + port_ndx, mc_id, lpm_payload, ingress_itf.vrf);
        if (rv != BCM_E_NONE) return rv;
    }

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        BCM_GPORT_LOCAL_SET(dst_gport, oport);
        if (is_device_or_above(unit, JERICHO2)){
            rv = dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport);
        } else {
            rv = dpp_kaps_raw_data_redirect(unit, lpm_payload, dst_gport);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    return rv;
}

struct ip_mc_s {
    int      is_ingress_mc; /* indicate if we're running the test using ingress MC or egress MC */
};

ip_mc_s g_ip_mc =
{
/* is ingress mc */
0
};

int print_level = 2;

int IP_ROUTE_NOF_REPLICATIONS = 4;

/*
 *  Creates an IP MC Group
 *  Here we create the actual MC group.
 *  We must run this function before creating the IP MC entry
 *  The creation of IPv4 and IPv6 MC Groups is identical
 *
 *  MC group can be ingress/egress/both, depending on the flags
 *  flags: BCM_MULTICAST_INGRESS_GROUP and/or BCM_MULTICAST_EGRESS_GROUP
 */
bcm_multicast_t
create_ip_mc_group(
    int unit,
    int flags,
    int ipmc_index,
    int dest_local_port_id,
    int nof_ports,
    int vlan_base,
    uint8 change_vlan)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dest_gport;
    int port_indx;
    bcm_multicast_t mc_id = ipmc_index;
    bcm_multicast_replication_t replications[IP_ROUTE_NOF_REPLICATIONS];

    flags |= is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_WITH_ID) :
        (BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);

    rv = bcm_multicast_destroy(unit, mc_id);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, in multicast_destroy, mc_id $mc_id\n");
        return rv;
    }

    rv = bcm_multicast_create(unit, flags, &mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_create, mc_id $mc_id flags $flags\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(dest_gport, dest_local_port_id);
    for(port_indx = 0; port_indx < nof_ports; ++port_indx) {
        bcm_multicast_replication_t_init(&replications[port_indx]);
        replications[port_indx].encap1 = vlan_base + ((change_vlan && port_indx >= 1) ? port_indx : 0);
        replications[port_indx].port = dest_gport + port_indx;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        flags = (flags & BCM_MULTICAST_INGRESS_GROUP) ? BCM_MULTICAST_INGRESS_GROUP: BCM_MULTICAST_EGRESS_GROUP;
    } else {
        flags = (flags & BCM_MULTICAST_INGRESS_GROUP) ? BCM_MULTICAST_INGRESS: 0;
    }

    if (nof_ports > 0) {
        rv = bcm_multicast_set(unit, mc_id, flags, nof_ports, replications);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_set\n");
            return rv;
        }
    }

    if(print_level >= 2) {
        printf("create MC group: 0x%08x\n", mc_id);
    }

    if (flags & BCM_MULTICAST_EGRESS_GROUP) {
        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_id, mc_id);
        if (rv != BCM_E_NONE) {
           printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
           return rv;
        }
    }

    return mc_id;
}


/******* Run example ******/
/*
 */
int
l3_ipmc_bridge_fallback_example(
    int unit,
    int vid_in_drop,
    int vid_in_flood,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3, 
    int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_port_flood_group_t flood_groups;
    cint_ipmc_info.vrf = 5;
    l3_ingress_intf ingr_itf;
    int idx;

    cint_ipmc_info.in_port = in_port;
    int out_ports[4];
    out_ports[0] = out_port1;
    out_ports[1] = out_port2;
    out_ports[2] = out_port3;
    out_ports[3] = in_port;


    rv = bcm_vlan_create(unit, vid_in_drop);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", vid_in_drop);
        return rv;
    }
    rv = bcm_vlan_create(unit, vid_in_flood);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", vid_in_flood);
        return rv;
    }

    bcm_vlan_port_t vlan_port;
    { /** VSI configuration to drop packet */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = cint_ipmc_info.in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.vsi = vid_in_drop;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.match_vlan = vid_in_drop;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        printf("port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
        rv = bcm_vlan_gport_add(unit, vid_in_drop, cint_ipmc_info.in_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_vlan_gport_add \n");
            return rv;
        }
    }
    { /** VSI configuration to flood packet */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = cint_ipmc_info.in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.vsi = vid_in_flood;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.match_vlan = vid_in_flood;
        vlan_port.ingress_network_group_id = 0;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        printf("port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
        rv = bcm_vlan_gport_add(unit, vid_in_flood, cint_ipmc_info.in_port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_vlan_gport_add \n");
            return rv;
        }
    }

    for (idx = 0; idx < 4; idx ++)
    {
        if (idx == 2)
        {
            continue;
        }
        rv = out_port_set(unit, out_ports[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set vm_port \n");
            return rv;
        }
    }

    bcm_gport_t outlif;
    bcm_vlan_port_t_init(&vlan_port);
    /* EEDB */
    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan = 100;
    vlan_port.port = out_ports[2];
    vlan_port.ingress_network_group_id = 0;
    vlan_port.egress_network_group_id = 3;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }
    outlif=vlan_port.vlan_port_id;
    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, vid_in_drop, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    cint_ipmc_info.intf_in_mac_address[5]++;
    rv = intf_eth_rif_create(unit, vid_in_flood, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vid_in_drop;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set vid_in_drop\n");
        return rv;
    }
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vid_in_flood;
    ingr_itf.vrf = cint_ipmc_info.vrf + 1;
    ingr_itf.flags |= BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set vid_in_flood\n");
        return rv;
    }

    bcm_gport_t flood_destination;
    bcm_port_flood_group_t_init(&flood_groups);
    BCM_GPORT_MCAST_SET(flood_destination, vid_in_flood);
    flood_groups.broadcast_group = flood_destination;
    if (*dnxc_data_get(unit, "l2", "feature", "bc_same_as_unknown_mc", NULL)) {
        flood_groups.unknown_multicast_group = flood_groups.broadcast_group;
    }
    rv = bcm_port_flood_group_set(unit, in_port, 0, &flood_groups);

    /*
     * 5. Create a multicast group
     */
    int flags;
    cint_ipmc_info.mc_group = vid_in_flood;
    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2);
    rv = bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    bcm_multicast_replication_t replications[4];
    set_multicast_replication(&replications[0], out_ports[0], -1);
    set_multicast_replication(&replications[1], out_ports[1], -1);
    set_multicast_replication(&replications[2], out_ports[2], (outlif & 0x3fffff));
    /** Add encap_id which is equal to the in_lif so that in_lif can equal out_lif */
    set_multicast_replication(&replications[3], out_ports[3], 0);

    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2), 4, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    int system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if ((system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, cint_ipmc_info.mc_group, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }


    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
           /* 
           use field processor to perform bridge fallback:
           for ipvx routing packets with no hit in forwarding, and for vsi profile:
           fwd to recycle port and build recycle header, update vsi and pop lif
           */
        rv = bcm_port_control_set(unit, recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_control_set with type bcmPortControlRecycleApp\n");
            return rv;
        }

        int recycle_entry_encap_id; 
        rv = create_recycle_entry(unit, &recycle_entry_encap_id); 
        if (rv != BCM_E_NONE)
        {
            printf("Error, create_recycle_entry\n");
            return rv;
        }

        /* configure vsi profile for vsis */
        int vsi_profile = 1; 
        rv = vlan__control_vlan_vsi_profile_set(unit, vid_in_flood, vsi_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vlan__control_vlan_vsi_profile_set\n");
            return rv;
        }
        rv = vlan__control_vlan_vsi_profile_set(unit, vid_in_drop, vsi_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vlan__control_vlan_vsi_profile_set\n");
            return rv;
        }

        rv = cint_field_bridge_fallback_main(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_main\n");
            return rv;
        }

        rv = cint_field_bridge_fallback_entry_add(unit,
            recycle_port, vsi_profile, BCM_L3_ITF_VAL_GET(recycle_entry_encap_id));
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_entry_add\n");
            return rv;
        }
    }

    return rv;
}

/** A destroy function for the field configuration used for bridge fallback. */
int
l3_ipmc_bridge_fallback_field_destroy(
    int unit)
{
    int vsi_profile = 1;
    int rv = BCM_E_NONE;
    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
        rv = cint_field_bridge_fallback_entry_destroy(unit, vsi_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_entry_add\n");
            return rv;
        }
        rv = cint_field_bridge_fallback_main_destroy(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_main\n");
            return rv;
        }
    }
    return rv;
}

int
dnx_example_ipv4_mc_uc_no_rpf(
    int unit,
    int in_port1,
    int in_port2,
    int out_port1,
    int out_port2,
    int vsi_loose,
    int vsi_strict)
{
    /*
     * Define needed variables
     */
    int rv = BCM_E_NONE;
    int vrf = 15;
    int idx;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int nof_entries = 8;
    int nof_hosts = 3;
    /** configure expected out destination MAC */
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0xFF, 0xFF, 0x01 };

    uint32 dip_mask = 0xFFFFFFFF;
    uint32 dip_addr = 0xE0020202;;

    int fec_start = 0xA005;
    int intf_out = 0x100;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;

    bcm_gport_t mc_gport_l;
    int ipmc_group = 6000;
    int encap_id = 6520;
    uint32 flags[8] =
        { BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, 0, 0, 0, 0 };
    uint32 rifs[8] = {vsi_loose, vsi_strict, vsi_loose, vsi_strict, vsi_loose, vsi_strict, vsi_loose, vsi_strict};
    uint32 fec_rifs[8] = {vsi_loose, vsi_strict, intf_out, intf_out, vsi_loose, vsi_strict, intf_out, intf_out};
    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_start);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    /*
     * 1. Configure traps for failed RPF lookup - SIP-based and Explicit.
     */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

    /*
     * 2a. Set In-Port1 to In ETh-RIF
     */
    rv = cint_ip_rpf_port_intf_set(unit, in_port1, vsi_loose, vrf, bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set\n");
        return rv;
    }
    /*
     * 2b. Set In-Port2 to In ETh-RIF
     */
    rv = cint_ip_rpf_port_intf_set(unit, in_port2, vsi_strict, vrf, bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set\n");
        return rv;
    }
    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }

    BCM_GPORT_MCAST_SET(mc_gport_l, cint_ipmc_info.mc_group);

    for (idx = 0; idx < nof_entries; idx++)
    {
        /*
         * 5. Define expected out DMAC
         */
        rv = l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only, encap_id\n");
            return rv;
        }
        /*
         * 6. Create FECs for host entry
         */
        rv = l3__egress_only_fec__create(unit, fec_start, fec_rifs[idx], 0, mc_gport_l, flags[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs[0]\n");
            return rv;
        }
        /*
         * 8. Create host IPMC entry
         */
        rv = add_ipv4_ipmc(unit, dip_addr, dip_mask, 0, 0, rifs[idx], vrf, 0, fec_start, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv4_ipmc - iter = %d \n", idx);
            return rv;
        }
        dip_addr++;
        encap_id++;
        next_hop_mac[lsb_mac]++;
        fec_start++;
    }

    for (idx = 0; idx < nof_hosts; idx++)
    {
        rv = l3__egress_only_fec__create(unit, fec_start, fec_rifs[4 + idx], 0, mc_gport_l, flags[4 + idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs[0]\n");
            return rv;
        }

        int _l3_itf;
        BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec_start);
        rv = add_host_ipv4(unit, cint_ipmc_info.host_sip + idx, vrf, _l3_itf, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_host_ipv4\n");
            return rv;
        }
        fec_start++;
    }

    return rv;
}

int
l3_ipmc_bridge_svl_example(
    int unit,
    int eth_rif,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3)
{
    int rv = BCM_E_NONE;
    int vrf = 1;
    int mc_id;
    int idx;
    int ipmc_group[2] = {1000, 1001};
    int intf_out = 150;
    int fwd_id = eth_rif;
    int c_sip = 0x456;
    int nof_mc_groups = 2;
    uint32 sip_addr = 0xC0800101;
    uint32 sip_mask = 0xFFFF0000;
    uint32 dip_addr = 0xE0020202;
    uint32 dip_mask = 0xFFFFFFFF;
    bcm_ip6_t dip_v6_addr = {0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
    bcm_ip6_t dip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_ip6_t sip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0};
    int ports[2][2] = {{out_port1, out_port2}, {out_port1, out_port3}};
    uint32 flags;
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];
    l3_ingress_intf ingr_itf;

    l3_ingress_intf_init(&ingr_itf);

    rv = bcm_vlan_create(unit, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", eth_rif);
        return rv;
    }
    /* 1. Set In-Port to Eth-RIF */
    rv = in_port_intf_set(unit, in_port, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /* 2. Set Out-Port default properties */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }
    rv = out_port_set(unit, out_port3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    /* 3. Create IN-RIF and set its properties */
    rv = intf_eth_rif_create(unit, eth_rif, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /* 4. Set ingress interface */
    ingr_itf.intf_id = eth_rif;
    ingr_itf.vrf = vrf;
    ingr_itf.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set intf_out\n");
        return rv;
    }

    for (mc_id = 0; mc_id < nof_mc_groups; mc_id++)
    {
        for (idx = 0; idx < cint_ipmc_info.nof_replications; idx++)
        {
            /*
             * 4.2 Set the replication.
             */
            set_multicast_replication(&replications[idx], ports[mc_id][idx], -1);

            /*
             * 4.3 Set some changes for the next replication.
             */
            intf_out++;
            cint_ipmc_info.intf_out_mac_address[5]++;
        }
        /*
         * 5. Create a multicast group
         */
        flags = BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID;
        rv = bcm_multicast_create(unit, flags, &ipmc_group[mc_id]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_create %d\n",i);
            return rv;
        }
        rv = bcm_multicast_add(unit, ipmc_group[mc_id], BCM_MULTICAST_INGRESS_GROUP | (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2), cint_ipmc_info.nof_replications, replications);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_ingress_add\n");
            return rv;
        }
        rv = add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, sip_mask, eth_rif, fwd_id, ipmc_group[mc_id], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_ipv4_ipmc_bridge\n");
            return rv;
        }
        rv = add_ipv6_ipmc_bridge(unit, dip_v6_addr, dip_v6_mask, sip_v6_addr, sip_v6_mask, 0, fwd_id, c_sip, ipmc_group[mc_id], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_ipv6_ipmc_bridge\n");
            return rv;
        }
        dip_addr ++;
        dip_v6_addr[15]++;
        sip_v6_addr[13]++;
        sip_mask = 0;
        c_sip++;
    }

    return rv;
}

/**
 * A routing example that presents the way in which in the case of a compatible multicast packet and no created RIF,
 * the packet can be forwarded based on an entry in the BRIDGE IPMC SVL tables.
 *
 * The functionality of the bcmSwitchL3McastL2 switch control is used to force updating the L2 Mc fwd type to IPV4MC bridge.
 */
int
l3_ipmc_bridge_default_fwd_type_example(
    int unit,
    int eth_rif,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3)
{
    int rv = BCM_E_NONE;
    int mc_id;
    int idx;
    int ipmc_group[2] = {1000, 1001};
    int intf_out = 150;
    int fwd_id = eth_rif;
    int nof_mc_groups = 2;
    uint32 sip_addr = 0xC0800101;
    uint32 sip_mask = 0xFFFF0000;
    uint32 dip_addr = 0xE0020202;
    uint32 dip_mask = 0xFFFFFFFF;
    bcm_mac_t dmac = {0x01, 0x00, 0x5E, 0x02, 0x02, 0x04};
    bcm_mac_t dmac2 = {0x33, 0x33, 0x00, 0x00, 0x11, 0x13};
    bcm_ip6_t dip_v6_addr = {0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11};
    bcm_ip6_t dip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_ip6_t sip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0};
    bcm_l2_addr_t l2_addr;
    int c_sip = 123;
    int ports[2][2] = {{out_port1, out_port2}, {out_port1, out_port3}};
    uint32 flags;
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];
    l3_ingress_intf ingr_itf;

    l3_ingress_intf_init(&ingr_itf);

    rv = bcm_switch_control_set(unit, bcmSwitchL3McastL2, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }
    /* 1. Set In-Port to Eth-RIF */
    rv = in_port_intf_set(unit, in_port, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /* 2. Set Out-Port default properties */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }
    rv = out_port_set(unit, out_port3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    rv = bcm_vlan_create(unit, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", eth_rif);
        return rv;
    }

    for (mc_id = 0; mc_id < nof_mc_groups; mc_id++)
    {
        for (idx = 0; idx < cint_ipmc_info.nof_replications; idx++)
        {
            /*
             * 4.2 Set the replication.
             */
            set_multicast_replication(&replications[idx], ports[mc_id][idx], -1);

            /*
             * 4.3 Set some changes for the next replication.
             */
            intf_out++;
            cint_ipmc_info.intf_out_mac_address[5]++;
        }
        /*
         * 5. Create a multicast group
         */
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
        rv = bcm_multicast_create(unit, flags, &ipmc_group[mc_id]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_create %d\n",idx);
            return rv;
        }
        rv = bcm_multicast_add(unit, ipmc_group[mc_id], BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2, cint_ipmc_info.nof_replications, replications);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_ingress_add\n");
            return rv;
        }
        rv = add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, sip_mask, 0, fwd_id, ipmc_group[mc_id], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_ipv4_ipmc_bridge\n");
            return rv;
        }
        rv = add_ipv6_ipmc_bridge(unit, dip_v6_addr, dip_v6_mask, sip_v6_addr, sip_v6_mask, 0, fwd_id, 0, ipmc_group[mc_id], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_ipv6_ipmc_bridge\n");
            return rv;
        }
        dip_addr ++;
        dip_v6_addr[15]++;
        sip_mask = 0;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchL3McastL2, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }
    eth_rif ++;
    /* Set In-Port to Eth-RIF */
    rv = in_port_intf_set(unit, out_port3, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    rv = bcm_vlan_create(unit, eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", eth_rif);
        return rv;
    }
    bcm_l2_addr_t_init(&l2_addr, dmac, eth_rif);
    l2_addr.l2mc_group = ipmc_group[0];
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add failed \n");
        return rv;
    }
    bcm_l2_addr_t_init(&l2_addr, dmac2, eth_rif);
    l2_addr.l2mc_group = ipmc_group[0];
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add failed \n");
        return rv;
    }

    return rv;
}

/*
 * In this example, we send a packet with MC IP 224.224.224.1, dedicated mymac and vlan 2, through the iport.
 *
 * We then expect the packet to multicast and exit on four different rifs if packet TTL is larger than the out-RIF TTL threshold:
 *      - oport + outrif 2 - will fallback to bridge and be filtered because of same interface filter
 *      - (oport + 1) + outrif 3 (vlan 3)
 *      - (oport + 2) + ourtif 4 (vlan 4)
 *      - (oport + 3) + ourtif 5 (vlan 5)
 */
/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_ip_route_scenarios.c
 * cint ../../src/./examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/./examples/sand/cint_ipmc_route_examples.c
 * cint
 * run_with_ports_ipv4_mc_example_with_ttl(0,200,201,25);
 * exit;
 *
 * tx 1 psrc=200 data=0x01005e60e00100000000000181000002080045000035000000001a001e66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * If packtet TTL=26 is larger than the out-RIF TTL threshold 25, then received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000030800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000040800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000050800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * tx 1 psrc=200 data=0x01005e60e001000000000001810000020800450000350000000018002066c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * packets on unit 0 should be drop because packtet TTL=24 is lower than the out-RIF TTL threshold 25
 *
 * tx 1 psrc=200 data=0x01005e60e001000000000001810000020800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * packets on unit 0 should be drop because packet TTL=25 is equal with the out-RIF TTL threshold 25
 */
int
run_with_ports_ipv4_mc_example_with_ttl(
    int unit,
    int iport,
    int oport,
    int ttl_threshold)
{
    uint32 rv = BCM_E_NONE;
    bcm_gport_t dst_gport;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
    int vlan = 2;
    int vrf = 2;
    int ipmc_index = 6000;
    int dest_local_port_id = iport;
    int source_local_port_id = oport;
    int nof_dest_ports = 4, port_ndx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};
    l3_ingress_intf ingress_itf;

    bcm_multicast_t mc_id = create_ip_mc_group(unit,
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP),
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE);

    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        rv = in_port_intf_set(unit, dest_local_port_id + port_ndx, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set vlan\n");
            return rv;
        }
        rv = out_port_set(unit, (dest_local_port_id+port_ndx));
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, out_port_intf_set intf_out\n");
            return rv;
        }

        rv = intf_eth_rif_create_with_ttl(unit, (vlan + port_ndx), mac_l3_ingress, ttl_threshold);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", (vlan+port_ndx), rv);
            return rv;
        }

        ingress_itf.vrf = vrf + port_ndx;
        ingress_itf.intf_id = vlan + port_ndx;
        rv = intf_ingress_rif_set(unit, &ingress_itf);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, vlan, vrf, rv);
            return rv;
        }

        if (lpm_raw_data_enabled) {
            rv = add_ipv4_ipmc_raw(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vlan+port_ndx), (vrf+port_ndx), 0, lpm_payload);
        } else {
            rv = add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vlan+port_ndx), (vrf+port_ndx), mc_id, 0, 0);
        }
        if (rv != BCM_E_NONE){
            return rv;
        }
    }

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled) {
        BCM_GPORT_LOCAL_SET(dst_gport, oport);
        if (is_device_or_above(unit, JERICHO2)){
            rv = dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport);
        } else {
            rv = dpp_kaps_raw_data_redirect(unit, lpm_payload, dst_gport);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    return rv;
}




