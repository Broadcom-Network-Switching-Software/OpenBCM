/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * IPMC with TTL filter procedure:
 * 1. Create 3 MC groups and their replications.
 * 2. Create 7 out-RIFs and config different ttl tresholds for each one of them.
 * 3. If packet TTL=21 is larger than the out-RIF TTL threshold 20, then the packet will not be filtered.
 * 4. If the packet TTL is lower or equal the out_RIF treshold , the packet will be filtered.
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_examples.c
  cint
  int rv;
  rv = dnx_ipv4_mc_example_with_ttl_extended(0, 200, 201, 202, 203);
  print rv;
  exit;
 *
 * TTL = 26
  tx 1 psrc=200 data=0x01005e60e00100000000000181000002080045000035000000001a111e66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * If packet TTL=26 is larger than the TTL threshold of the 3 Out-RIFs, then the received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x01005e60e001000000111111810000140800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 * Source port: 200, Destination port: 202
 * Data: 0x01005e60e001000000111122810000150800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 * Source port: 200, Destination port: 203
 * Data: 0x01005e60e001000000111133810000160800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 *
 * TTL=21
  tx 1 psrc=200 data=0x01005e60e001000000000001810000020800450000350000000015111e66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * If packet TTL=21 then the received packets on unit 0 should be on port 201 with Out-RIF TTL=20, the packets destined for port 202 and 203 should be dropped
 *
 * Source port: 200, Destination port: 201
 * Data: 0x01005e60e001000000111111810000140800450000350000000014111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 *
 * Test Scenario - end
 */

int print_level = 2;
struct ip_mc_s {
    int      is_ingress_mc; /* indicate if we're running the test using ingress MC or egress MC */
};

ip_mc_s g_ip_mc =
{
/* is ingress mc */
0
};
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
int
create_ip_mc_group(
    int unit,
    int flags,
    int ipmc_index,
    int dest_local_port_id,
    int nof_ports,
    int vlan_base,
    uint8 change_vlan,
    bcm_multicast_t *ret_ipmc_index)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dest_gport;
    int port_indx;
    int nof_reps = 0;
    bcm_multicast_t mc_id = ipmc_index;
    bcm_multicast_replication_t replications[IP_ROUTE_NOF_REPLICATIONS];
    char error_msg[200]={0,};

    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        rv = bcm_multicast_group_destroy(unit, flags, mc_id);
    } else { 
        rv = bcm_multicast_destroy(unit, mc_id);
    }
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, in multicast_destroy, mc_id %d\n", mc_id);
        return rv;
    }

    flags |= BCM_MULTICAST_WITH_ID;
    snprintf(error_msg, sizeof(error_msg), "mc_id %d flags %d", mc_id, flags);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &mc_id), error_msg);

    BCM_GPORT_LOCAL_SET(dest_gport, dest_local_port_id);
    for(port_indx = 0; port_indx < nof_ports; ++port_indx) {
        bcm_multicast_replication_t_init(&replications[port_indx]);
        replications[port_indx].encap1 = vlan_base + ((change_vlan && port_indx >= 1) ? port_indx : 0);
        replications[port_indx].port = dest_gport + port_indx;
        nof_reps ++;
    }

    flags = (flags & ~BCM_MULTICAST_WITH_ID);
    if (nof_ports > 0) {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, mc_id, flags, nof_reps, replications), "");
    }

    if(print_level >= 2) {
        printf("create MC group: 0x%08x\n", mc_id);
    }

    if (flags & BCM_MULTICAST_EGRESS_GROUP) {
        BCM_IF_ERROR_RETURN_MSG(multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_id, mc_id), "");
    }
    *ret_ipmc_index = mc_id;

    return BCM_E_NONE;
}


/* Set L3McastL2 per RIF option. */
int set_rif_ipmc_l2 (
    int unit,
    bcm_if_t intf,
    int enable)
{
    bcm_l3_ingress_t ingress_intf;

    bcm_l3_ingress_t_init(&ingress_intf);

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_get(unit, intf, &ingress_intf), "");

    if (enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_L3_MCAST_L2;
    }
    else {
        ingress_intf.flags &= ~BCM_L3_INGRESS_L3_MCAST_L2;
    }

    ingress_intf.flags |= BCM_L3_INGRESS_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, &ingress_intf, intf), "");

    return BCM_E_NONE;
}

int lpm_raw_data_enabled =0;         /* use raw data payload in KAPs LPM */
int lpm_payload = 0x1234;            /* raw payload in Kaps*/

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
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/l3/cint_ip_route_scenarios.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_dnx_ipmc_route_examples.c
  cint
  run_with_ports_ipv4_mc_example_with_ttl(0,200,201,25);
  exit;
 *
  tx 1 psrc=200 data=0x01005e60e00100000000000181000002080045000035000000001a001e66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * If packtet TTL=26 is larger than the out-RIF TTL threshold 25, then received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000030800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000040800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *  Source port: 0, Destination port: 0
 *  Data: 0x01005e60e00100000000ab1d810000050800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
  tx 1 psrc=200 data=0x01005e60e001000000000001810000020800450000350000000018002066c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * packets on unit 0 should be drop because packtet TTL=24 is lower than the out-RIF TTL threshold 25
 *
  tx 1 psrc=200 data=0x01005e60e001000000000001810000020800450000350000000019001f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * packets on unit 0 should be drop because packet TTL=25 is equal with the out-RIF TTL threshold 25
 * Test Scenario - end
 *
 */
int
run_with_ports_ipv4_mc_example_with_ttl(
    int unit,
    int iport,
    int oport,
    int ttl_threshold)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dst_gport;
    char error_msg[200]={0,};

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
    dnx_utils_l3_eth_rif_s eth_rif;
    uint32 l3_ingress_flags = 0;
    bcm_multicast_t mc_id = 0;
    uint32 mc_flags = 0;

    if (*dnxc_data_get(unit, "l3", "feature", "public_routing_support", NULL))
    {
        l3_ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    }

    if (!lpm_raw_data_enabled)
    {
        if (g_ip_mc.is_ingress_mc == TRUE) {
            if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL))
            {
                mc_flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC);
            }
            else
            {
                mc_flags = BCM_MULTICAST_INGRESS_GROUP;
            }
        }
        else
        {
            mc_flags = BCM_MULTICAST_EGRESS_GROUP;
        }

        BCM_IF_ERROR_RETURN_MSG(create_ip_mc_group(unit, mc_flags, ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE, &mc_id), "");
    }

    for (port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx)
    {
        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, dest_local_port_id + port_ndx, vlan), "vlan");

        rv = out_port_set(unit, (dest_local_port_id + port_ndx));
        if (rv == BCM_E_EXISTS) {
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN_MSG(rv, "");

        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif, (vlan + port_ndx), 0, l3_ingress_flags, mac_l3_ingress, (vrf + port_ndx));

        if (ttl_threshold != 0)
        {
            eth_rif.ttl_valid = 1;
            eth_rif.ttl = ttl_threshold;
        }

        rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif);
        if (rv == BCM_E_EXISTS) {
            rv = BCM_E_NONE;
        }
        snprintf(error_msg, sizeof(error_msg), "intf_in = %d", vlan + port_ndx);
        BCM_IF_ERROR_RETURN_MSG(rv, error_msg);

        if (lpm_raw_data_enabled)
        {
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_raw(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vlan + port_ndx), (vrf + port_ndx), 0, lpm_payload), "");
        }
        else
        {
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0xFFFFFFFF, (vrf + port_ndx), (vlan + port_ndx), mc_id, 0, 0), "");
        }
    }

    /* redirect the packet in the pmf acording to the raw payload received in the kaps*/
    if (lpm_raw_data_enabled)
    {
        BCM_GPORT_LOCAL_SET(dst_gport, oport);
        BCM_IF_ERROR_RETURN_MSG(dnx_kaps_raw_data_redirect(unit, lpm_payload, 0, 0, dst_gport), "");
    }

    return BCM_E_NONE;
}

int
run_with_ports_ipv4_mc_example(
    int unit,
    int iport,
    int oport)
{
    return run_with_ports_ipv4_mc_example_with_ttl(unit,iport,oport,0);
}

/*
 * Adding ports to ingress MC group with encapsulation
 *
 */
int
add_ingress_multicast_forwarding_with_encap (
    int unit,
    int ipmc_index,
    int *ports,
    int nof_ports,
    int vlan)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_if_t encap_id;
    bcm_vlan_port_t vp;
    bcm_multicast_replication_t replications[8];
    int flags = 0;
    int action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_port_tag_format_class_t tag_format_stag = 2;

    for (i=0; i < nof_ports; i++)
    {
        bcm_vlan_port_t_init(&vp);
        vp.port = ports[i];
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.match_vlan = vlan;

        rv = bcm_vlan_port_find(unit, &vp);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vp), "");
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_vlan_encap_get(unit, ipmc_index, 0/*not in use in the function*/, vp.vlan_port_id, &encap_id), "");

        bcm_multicast_replication_t_init(&replications[i]);
        replications[i].encap1 = encap_id;
        BCM_GPORT_LOCAL_SET(replications[i].port, ports[i]);
    }

    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    } else {
        flags = BCM_MULTICAST_INGRESS_GROUP;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, ipmc_index, flags, nof_ports, replications), "");

    return BCM_E_NONE;
}

/* Creates a L2 forwarding entry. The entry will be accessed by LEM<FID,DA> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a destination MAC address.
 */
int
create_l2_forwarding_entry_da (
    int unit,
    bcm_mac_t mac,
    int l2mc_group,
    int vlan)
{

    bcm_l2_addr_t l2_addr;

    /* Configurations relevant for LEM<FID,DA> lookup*/
    bcm_l2_addr_t_init(&l2_addr,mac,vlan);

    l2_addr.flags = BCM_L2_MCAST | BCM_L2_STATIC;
    l2_addr.l2mc_group = l2mc_group;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit,&l2_addr), "");

    return BCM_E_NONE;
}

/*
 * This cint is used to test the V4MC with IPMC enable case handling in per RIF configuration
 * and to make sure that the IPMC disable case is forward by L2 MC after deleting RIF.
 *
 * Section 1: Configure IPMC L3 forwarding
 * Section 2: Forward by L2 MC after deleting RIF
*/
int
ipmc_flows_ipmc_per_rif (
    int unit,
    int inP,
    int outP,
    int outP2,
    int outP3,
    int vlan)
{
    int ipmc_l2_mc_group = 6001;
    int forwarding_ports_2[3] = {outP,outP2,outP3};
    bcm_mac_t mac1 = {0x01, 0x00, 0x5E, 0x02, 0x02, 0x02};
    uint32 mc_flags = 0;

    /*Case 1 - Configure L3 IPMC forwarding */
    BCM_IF_ERROR_RETURN_MSG(basic_ipmc_example_inner(unit, vlan, inP, outP, outP2, 0, 0), "");
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        mc_flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID);
    } else {
        mc_flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID);
    }

    /* Case 2 - L2 MC Group */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, mc_flags, &ipmc_l2_mc_group), "with l2mc_group");
    /* Case 2 - Set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM: <FID, DA> lookup */
    BCM_IF_ERROR_RETURN_MSG(add_ingress_multicast_forwarding_with_encap(unit, ipmc_l2_mc_group, forwarding_ports_2, 3, vlan), "");
    /*Case 2 - Add IPMC entry LEM: <FID, DA> */
    BCM_IF_ERROR_RETURN_MSG(create_l2_forwarding_entry_da(unit, mac1, ipmc_l2_mc_group, vlan), "");
    return BCM_E_NONE;

}

/*
 * This function sets the RPF traps for the case of MC RPF.
 * These include:
 *    - bcmRxTrapMcUseSipRpfFail - trap for packets that are failing a SIP-based RPF check.
 *    - bcmRxTrapMcExplicitRpfFail - trap for packets that are failing an Explicit RPF check.
 */
int
l3_dnx_ip_mc_rpf_config_traps(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int rpf_mc_sip_trap = 0;
    int rpf_mc_explicit_trap = 0;
    /*
     * RPF types: bcmRxTrapMcUseSipRpfFail - Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP'.
     * bcmRxTrapMcExplicitRpfFail - RPF-Entry. Expected-InRIF is not equal to packet InRIF. Relevant when FEC-Entry.
     */

    /*
     * change dest port for trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set mc sip-based and explicit RPF to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap), "");
    rv = bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap),
            "in trap create, trap bcmRxTrapMcUseSipRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config),
            "in trap set rpf_mc_sip_trap");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap), "rpf_mc_explicit_trap");
    rv = bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap),
            "in trap create, trap bcmRxTrapMcExplicitRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config), "in trap set rpf_mc_explicit_trap");
    }

    return BCM_E_NONE;
}

/* The following cint is used to test an explicit MC RPF (pass and fail) and a SIP based MC RPF (pass and fail).*/
int NOF_BASIC_MC_GROUP = 4;
int bcm_l3_ip6_mc_rpf(int unit, int port0, int port1, int port2, int port3) {
    bcm_if_t frwdl3egid;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x66, 0x00, 0x00, 0x00, 0x66, 0x00};
    int  vlan_bais_in    = 0x100;
    int  vlan_offset_in  = 0x100;
    int ipmc_index_offset;
    int ipmc_index_upper_bound;
    int explicit_count = 2;
    int sip_count;
    int i, j, found, l3egid, flags, vlan;
    uint8 mac_last_byte[NOF_BASIC_MC_GROUP];
    uint8 is_mc_rpf_sip_based_supported;
    uint16 compressed_sip = 123;
    int fec_flags2;
    int exPorts[4] = {port0 , port1, port2 , port3};
    int ipmc_group[NOF_BASIC_MC_GROUP];
    int out_ports[NOF_BASIC_MC_GROUP] = {3, 3, 2, 2};
    bcm_multicast_replication_t replications[NOF_BASIC_MC_GROUP];
    bcm_l3_egress_t l3_eg;
    bcm_if_t l3egid_null;
    l3_ingress_intf ingr_itf;
    int fec[5];
    char error_msg[200]={0,};
    ipmc_index_offset = 0;
    ipmc_index_upper_bound = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));
    is_mc_rpf_sip_based_supported = *(dnxc_data_get(unit,  "l3", "fec", "mc_rpf_sip_based_supported", NULL));
    sip_count = is_mc_rpf_sip_based_supported ? 2 : 0;
    fec_flags2 = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;

    /*Different last byte of the MAC address for the interfaces*/
    mac_last_byte[0] = 0x10;
    for (i = 1; i < NOF_BASIC_MC_GROUP; i++) {
        mac_last_byte[i] = mac_last_byte[i - 1] + 0x20;
    }

    /*Create traps to catch the explicit MC RPF and SIP MC RPF fails*/
    BCM_IF_ERROR_RETURN_MSG(l3_dnx_ip_mc_rpf_config_traps(unit), "");

    /*create different IPMC groups */
    sal_srand(sal_time());
    for (i = 0; i < NOF_BASIC_MC_GROUP; i++) {  /* picking random values for the mc groups */
        found = 1;
        while (found){ /* verifying that we don't get a previous value */
            found = 0;
            ipmc_group[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
            for(j = 0 ; j < i - 1 ;j++) {
                if(ipmc_group[i] == ipmc_group[j]) {found = 1;}
            }
        }
    }

    for (i = 0; i < NOF_BASIC_MC_GROUP; i++) {
        vlan = vlan_bais_in + (i * vlan_offset_in);
        mac_l3_ingress_add[5] = mac_last_byte[i];

        snprintf(error_msg, sizeof(error_msg), "port(0x%08x) to vlan(%d)",  port0, vlan);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan, port0, 0), error_msg);

        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vlan, mac_l3_ingress_add), "");

        ingr_itf.intf_id = ingr_itf.vrf = vlan;
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        }

        /*Create MC groups*/
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &(ipmc_group[i])), "");

        /*Add ports to each MC group*/
        for (j = 0; j < out_ports[i]; j++) {
            bcm_multicast_replication_t_init(&replications[j]);
            replications[j].port = exPorts[(j + 1) % 4];
            replications[j].encap1 = vlan;
        }

        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, ipmc_group[i], flags, out_ports[i], replications), "");
    }

    bcm_ipmc_addr_t data;

    bcm_ip6_t route = {0xFF,0xFF,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,0,0,0};
    bcm_ip6_t supported_sip  = {0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16};
    bcm_ip6_t fwd_sip  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x23};
    bcm_ip6_t full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    /*
     * Create two explicit MC RPF FEC - one with an incorrect IN-RIF and one with a correct one.
     * Create two SIP MC RPF FEC
     */

    for(i = 0; i < explicit_count + sip_count; i++) {
        vlan = vlan_bais_in + (i * vlan_offset_in);
        mac_l3_engress_add[5] = mac_last_byte[i];
        bcm_l3_egress_t_init(&l3_eg);
        sal_memcpy(l3_eg.mac_addr, mac_l3_engress_add, 6);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3_eg, &l3egid_null), "");

        if (i < explicit_count) {
            /*Set the first entry with an incorrect in-rif */
            l3_eg.intf = vlan + (1 - i) * 5;
            l3_eg.flags = BCM_L3_MC_RPF_EXPLICIT;
        } else {
            l3_eg.flags = BCM_L3_MC_RPF_SIP_BASE;
        }

        BCM_GPORT_MCAST_SET(l3_eg.port, ipmc_group[i]);
        l3_eg.encap_id = 0;
        l3_eg.flags2 = fec_flags2;
        snprintf(error_msg, sizeof(error_msg), "create egress object, unit=%d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_eg, &fec[i]), error_msg);

        frwdl3egid = fec[i];

        bcm_ipmc_addr_t_init(&data);
        if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
        {
            sal_memcpy(data.s_ip6_addr, fwd_sip, 16);
            sal_memcpy(data.s_ip6_mask, full_mask, 16);
            data.mc_ip_addr = compressed_sip;
            data.mc_ip_mask = 0xffff;
            data.flags = BCM_IPMC_IP6;
            data.group = 0;
            data.l3a_intf = frwdl3egid;
            printf("frwdl3egid 0x%x \n",frwdl3egid);
            BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit,&data), "");

            bcm_ipmc_addr_t config;
            bcm_ipmc_addr_t_init(&config);

            config.mc_ip6_addr = route;
            config.mc_ip6_mask = full_mask;
            config.vrf = vlan;
            config.mc_ip_addr = compressed_sip;
            config.mc_ip_mask = 0xffff;
            BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_config_add(unit, &config), "");
            compressed_sip += 0x10;
        }
        else
        {
            sal_memcpy(data.mc_ip6_addr, route, 16);
            sal_memcpy(data.mc_ip6_mask, full_mask, 16);
            data.flags = BCM_IPMC_IP6;
            data.vid = data.vrf = vlan;
            data.group = 0;
            data.l3a_intf = frwdl3egid;
            if (is_jericho2_kbp_ipv6_enabled(unit))
            {
                /* External lookup enabled on JR2 */
                data.flags |= BCM_IPMC_TCAM;
            }
            printf("frwdl3egid 0x%x \n",frwdl3egid);
            BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit,&data), "");
        }
        route[15] += 0x10;
    }

    if (is_mc_rpf_sip_based_supported)
    {
        mac_l3_engress_add[5] = 11;
        bcm_l3_egress_t_init(&l3_eg);
        sal_memcpy(l3_eg.mac_addr, mac_l3_engress_add, 6);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3_eg, &l3egid_null), "create egress object");

        /*Creating a FEC to have a payload for the SIP check (won't be used for actual forwarding)*/
        l3_eg.flags = BCM_L3_MC_RPF_SIP_BASE;
        l3_eg.intf = vlan_bais_in + (3 * vlan_offset_in);
        l3_eg.encap_id = 0;
        l3_eg.port = port1;
        snprintf(error_msg, sizeof(error_msg), "create egress object, unit=%d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_eg, &fec[4]), error_msg);

        frwdl3egid = fec[4];

        bcm_l3_route_t l3route;
        bcm_l3_route_t_init(&l3route);
        l3route.l3a_flags =  BCM_L3_IP6;
        sal_memcpy(l3route.l3a_ip6_net, supported_sip, 16);
        sal_memcpy(l3route.l3a_ip6_mask, full_mask, 16);
        l3route.l3a_vrf = vlan_bais_in + (3 * vlan_offset_in);
        if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
        {
            /* External lookup enabled on JR2 */
            l3route.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
        }
        /* dest is FEC + OutLif */
        l3route.l3a_intf = frwdl3egid;
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, &l3route), "");
    }
    return BCM_E_NONE;
}


int
ipv4_mc_to_trap_example(
    int unit,
    int in_port,
    int out_port)
{

    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    /* compatible MAC 01:00:5E:60:e0:01 */
    bcm_ip_t src_ip = 0x0; /* masked */

    bcm_gport_t mc_gport_drop; /* This mc id trap to BCM_GPORT_BLACK_HOLE */
    bcm_gport_t mc_gport_2_cpu; /* This mc id trap to BCM_GPORT_LOCAL_CPU */
    bcm_rx_trap_config_t config_trap_drop, config_trap_cpu;
    int trap_id_handle_drop, trap_id_handle_cpu;
    int trap_strength;

    int idx, eg_itf_index = 2;
    int vrf;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    int ing_intf_l;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    l3_ingress_intf ingr_itf;
    int fec_id = 0;

    l3_ingress_intf_init(&ingr_itf);
    vrf = vlan_l;

    /* 1. Configure two traps: dest port of one trap is drop, dest of another trap is cpu */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0/* flags */, bcmRxTrapUserDefine, &trap_id_handle_drop), "bcmRxTrapUserDefine");

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0/* flags */, bcmRxTrapUserDefine, &trap_id_handle_cpu), "bcmRxTrapUserDefine");

    bcm_rx_trap_config_t_init(&config_trap_drop);
    config_trap_drop.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config_trap_drop.trap_strength = 0; /* FOR USER DEFINE set as part of destination */
    config_trap_drop.dest_port = BCM_GPORT_BLACK_HOLE;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_id_handle_drop, &config_trap_drop), "");

    bcm_rx_trap_config_t_init(&config_trap_cpu);
    config_trap_cpu.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config_trap_cpu.trap_strength = 0; /* FOR USER DEFINE set as part of destination */
    BCM_GPORT_LOCAL_SET(config_trap_cpu.dest_port, 0);
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_id_handle_cpu, &config_trap_cpu), "");

    /* Associate the traps with the gports */
    trap_strength = 15;
    BCM_GPORT_TRAP_SET(mc_gport_drop, trap_id_handle_drop, trap_strength, 0);
    BCM_GPORT_TRAP_SET(mc_gport_2_cpu, trap_id_handle_cpu, trap_strength, 0);

    /* 2. Set In-Port to Eth-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, vlan_l), "");

    /* 3. Set Out-Port default properties */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), "");

    /* 4. Create IN-RIF and set its properties */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vlan_l, mac_address_l), "");
    ing_intf_l = vlan_l;

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vlan_r, mac_address_r), "");

    /* 5. Set ingress interface */
    ingr_itf.intf_id = ing_intf_l;
    ingr_itf.vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

    /* 6. Create the IP MC entry */
    for (idx = 0; idx < eg_itf_index; ++idx) {
        if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
        {
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_to_port(unit, mc_ip, 0xffffffff, src_ip, 0x0, vlan_l, vrf, ((idx == 0) ? mc_gport_drop : mc_gport_2_cpu), 0), "");
        }
        else
        {
            /** Create FEC pointing to trap ID */
            int fec_flags2 = 0;
            if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
            {
                fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
            }

            if (idx == 0)
            {
                BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");
            }
            else
            {
                fec_id += 2;
            }

            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_id, 0, 0, ((idx == 0) ? mc_gport_drop : mc_gport_2_cpu), 0, fec_flags2, 0, 0, &fec_id),
                "create egress object FEC only");

            /** Create IPMC entry in LPM pointing to FEC */
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, mc_ip, 0xffffffff, src_ip, 0, vlan_l, vrf, 0, fec_id, 0), "to LPM table");
        }

        mc_ip++;
    }

    return BCM_E_NONE;
}




/*
 * MC routing functionality with bridge fallback configuration is verified.
 * If there is not a successful IPMC lookup and VSI indicates bridge fallback is enabled, packet will be flooded.
 *
 * Test Scenario
 *
 * Test Scenario - start
  config add tm_port_header_type_in_50=INJECTED
  config add tm_port_header_type_out_50=ETH
  config add ucode_port_50=RCY.10:core_0.50
 *
 * #if device has two cores, include the following properties as well:
  config add tm_port_header_type_in_51=INJECTED
  config add tm_port_header_type_out_51=ETH
  config add ucode_port_51=RCY.11:core_1.51
 *
  tr 141
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_util_rch.c
  cint ../../../../src/examples/dnx/field/cint_field_bridge_fallback.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
  cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_examples.c
  cint
  l3_ipmc_bridge_fallback_example(0,11,12,200,201,202,203,50,51);
  exit;
 *
 * # The following packet will hit VSI with bridge fallback configuration. Packet will be flooded.
  tx 1 psrc=200 data=0x01005e0202020000070001009100000b080045000035000000006400b343c0800101e0020202000102030405060708090a0b0c0d0e0f
 *
 * Expected out packets:
 * on port 201: 0x01005E0202020000070001009100000B080045000035000000006400B343C0800101E0020202000102030405060708090A0B
 * on port 202: 0x01005E0202020000070001009100000B080045000035000000006400B343C0800101E0020202000102030405060708090A0B
 * on port 203: 0x01005E0202020000070001009100000B080045000035000000006400B343C0800101E0020202000102030405060708090A0B
 * Test Scenario - end
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
    int recycle_port_core_0,
    int recycle_port_core_1)
{
    int nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    bcm_port_flood_group_t flood_groups;
    cint_ipmc_info.vrf = 5;
    l3_ingress_intf ingr_itf;
    int idx;
    int vid_out = 25;
    char error_msg[200]={0,};
    cint_ipmc_info.in_port = in_port;
    int out_ports[4];
    out_ports[0] = out_port1;
    out_ports[1] = out_port2;
    out_ports[2] = out_port3;
    out_ports[3] = in_port;

    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", vid_in_drop);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in_drop), error_msg);
    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", vid_in_flood);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid_in_flood), error_msg);

    bcm_vlan_port_t vlan_port;
    { /** VSI configuration to drop packet */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = cint_ipmc_info.in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.vsi = vid_in_drop;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.match_vlan = vid_in_drop;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
        printf("port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in_drop, cint_ipmc_info.in_port, 0), "");
    }
    { /** VSI configuration to flood packet */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = cint_ipmc_info.in_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.vsi = vid_in_flood;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.match_vlan = vid_in_flood;
        vlan_port.ingress_network_group_id = 0;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
        printf("port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid_in_flood, cint_ipmc_info.in_port, 0), "");
    }

    for (idx = 0; idx < 4; idx ++)
    {
        if (idx == 2)
        {
            continue;
        }
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_ports[idx]), "vm_port");
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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);
    outlif=vlan_port.vlan_port_id;
    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vid_in_drop, cint_ipmc_info.intf_in_mac_address), "intf_in");
    cint_ipmc_info.intf_in_mac_address[5]++;
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vid_in_flood, cint_ipmc_info.intf_in_mac_address), "intf_in");
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vid_out, cint_ipmc_info.intf_out_mac_address), "intf_in");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vid_in_drop;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "vid_in_drop");
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vid_in_flood;
    ingr_itf.vrf = cint_ipmc_info.vrf + 1;
    ingr_itf.flags |= BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "vid_in_flood");

    /*
     * 5. Create a multicast group
     */
    int flags;
    cint_ipmc_info.mc_group = vid_in_flood;
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID;
    } else {
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group), "");

    bcm_multicast_replication_t replications[4];
    set_multicast_replication(&replications[0], out_ports[0], -1);
    set_multicast_replication(&replications[1], out_ports[1], -1);
    set_multicast_replication(&replications[2], out_ports[2], (outlif & 0x3fffff));
    /** Add encap_id which is equal to the in_lif so that in_lif can equal out_lif */
    set_multicast_replication(&replications[3], out_ports[3], 0);
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    } else {
        flags = BCM_MULTICAST_INGRESS_GROUP;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, flags, 4, replications), "");

    int system_headers_mode = (*dnxc_data_get(unit, "headers", "system_headers", "system_headers_mode", NULL));
    if ((system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, cint_ipmc_info.mc_group, 0, nof_cores, mreps), "");
    }


    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
           /*
           use field processor to perform bridge fallback:
           for ipvx routing packets with no hit in forwarding, and for vsi profile:
           fwd to recycle port and build recycle header, update vsi and pop lif
           */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, recycle_port_core_0, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback),
            "with type bcmPortControlRecycleApp");
        /** Set port class in order for PMF to crop RCH ad IRPP */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,recycle_port_core_0,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");
        if (nof_cores > 1)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, recycle_port_core_1, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback),
                "with type bcmPortControlRecycleApp");
            /** Set port class in order for PMF to crop RCH ad IRPP */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,recycle_port_core_1,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, in_port, bcmPortControlSystemPortInjectedMap, 1),
            "with type bcmPortControlSystemPortInjectedMap");


        /* configure vsi profile for vsis */
        int vsi_profile = 1;
        BCM_IF_ERROR_RETURN_MSG(vlan__control_vlan_vsi_profile_set(unit, vid_in_flood, vsi_profile), "");
        BCM_IF_ERROR_RETURN_MSG(vlan__control_vlan_vsi_profile_set(unit, vid_in_drop, vsi_profile), "");

        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_main(unit), "");

        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_entry_add(unit,
            nof_cores, recycle_port_core_0, recycle_port_core_1, vsi_profile), "");
    }

    return BCM_E_NONE;
}

/** A destroy function for the field configuration used for bridge fallback. */
int
l3_ipmc_bridge_fallback_field_destroy(
    int unit,
    int nof_cores)
{
    int vsi_profile = 1;
    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_entry_destroy(unit, nof_cores, vsi_profile), "");
        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_main_destroy(unit), "");
    }
    return BCM_E_NONE;
}

/**
 * A routing example that presents the way in which in the case of a compatible multicast packet and no created RIF,
 * the packet can be forwarded based on an entry in the BRIDGE IPMC SVL tables.
 *
 * The functionality of the bcmSwitchL3McastL2 and bcmSwitchL3IP6McastL2 switch control is used to force updating the L2 Mc fwd type to IPV4MC bridge.
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
    char error_msg[200]={0,};
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
    int compressed_val = 0x552;
    int ports[2][2] = {{out_port1, out_port2}, {out_port1, out_port3}};
    uint32 flags, flags_add;
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];
    l3_ingress_intf ingr_itf;

    l3_ingress_intf_init(&ingr_itf);

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL3McastL2, 1), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL3IP6McastL2, 1), "");
    /* 1. Set In-Port to Eth-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, eth_rif), "");

    /* 2. Set Out-Port default properties */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port3), "");
    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, eth_rif), error_msg);

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
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC | BCM_MULTICAST_WITH_ID);
            flags_add = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC);
        } else {
            flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID);
            flags_add = BCM_MULTICAST_INGRESS_GROUP;
        }
        snprintf(error_msg, sizeof(error_msg), "%d", idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &ipmc_group[mc_id]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, ipmc_group[mc_id], flags_add, cint_ipmc_info.nof_replications, replications), "");
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, sip_mask, fwd_id, 0, ipmc_group[mc_id], 0, 0), "");
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, dip_v6_mask, sip_v6_addr, sip_v6_mask, 0, fwd_id, compressed_val, ipmc_group[mc_id], 0, 0), "");
        dip_addr ++;
        dip_v6_addr[15]++;
        if (!(*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL))))
        {
            compressed_val = 0;
        }
        else
        {
            compressed_val ++;
        }
        sip_v6_mask[15] = 0;
        sip_mask = 0;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL3McastL2, 0), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL3IP6McastL2, 0), "");
    eth_rif ++;
    /* Set In-Port to Eth-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, out_port3, eth_rif), "");
    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, eth_rif), error_msg);
    bcm_l2_addr_t_init(&l2_addr, dmac, eth_rif);
    l2_addr.l2mc_group = ipmc_group[0];
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    bcm_l2_addr_t_init(&l2_addr, dmac2, eth_rif);
    l2_addr.l2mc_group = ipmc_group[0];
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    return BCM_E_NONE;
}

/*
 * MC routing functionality with IPMC bridge forwarding configuration is verified.
 * The cint presents an example on how to configure SVL and IVL IPMC bridge forwarding entries.
 *
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/cint_dnx_ipmc_route_examples.c
  cint
  l3_ipmc_bridge_ivl_svl_example(0,20,200,201,202,203,1);
  exit;
 * Test Scenario - end
 *
 */
int
l3_ipmc_bridge_ivl_svl_example(
    int unit,
    int eth_rif,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int is_ivl)
{
    char error_msg[200]={0,};
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
    bcm_vlan_control_vlan_t control;
    int ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);
    l3_ingress_intf_init(&ingr_itf);

    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, eth_rif), error_msg);

    /* IVL functionality */
    if (ivl_feature_support == 1 && is_ivl)
    {
        /**Configure IVL mode for VSI=40*/
        control.flags2 = BCM_VLAN_FLAGS2_IVL;
        control.broadcast_group = eth_rif;
        control.unknown_multicast_group = eth_rif;
        control.unknown_unicast_group = eth_rif;
        control.forwarding_vlan = eth_rif;
        snprintf(error_msg, sizeof(error_msg), "in vsi= %d", eth_rif);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, eth_rif, control), error_msg);
    }

    /* 1. Set In-Port to Eth-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, eth_rif), "");

    /* 2. Set Out-Port default properties */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port3), "");

    /* 3. Create IN-RIF and set its properties */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, eth_rif, cint_ipmc_info.intf_in_mac_address), "");

    /* 4. Set ingress interface */
    ingr_itf.intf_id = eth_rif;
    ingr_itf.vrf = vrf;
    ingr_itf.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

    if (!is_ivl)
    {
        eth_rif = 0;
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
        if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
        } else {
            flags = BCM_MULTICAST_INGRESS_GROUP;
        }
        snprintf(error_msg, sizeof(error_msg), "%d", mc_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | flags, &ipmc_group[mc_id]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, ipmc_group[mc_id], flags, cint_ipmc_info.nof_replications, replications), "");
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, sip_mask, fwd_id, eth_rif, ipmc_group[mc_id], 0, 0), "");

        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, dip_v6_mask, sip_v6_addr, sip_v6_mask, 0, fwd_id, c_sip, ipmc_group[mc_id], 0, 0), "");
        dip_addr ++;
        dip_v6_addr[15]++;
        sip_v6_addr[13]++;
        sip_mask = 0;
        c_sip++;
    }

    return BCM_E_NONE;
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
    int rv = BCM_E_NONE;
    int vrf = 15;
    int idx;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int fec_start;
    int intf_out = 0x100;
    int encap_id = 6520;
    int nof_ipmc_entries = 8;
    int nof_host_entries = 3;
    int fec_flags2 = 0;
    int _l3_itf;
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0xFF, 0xFF, 0x01 }; /** Expected out destination MAC */
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 dip_addr = 0xE0020202;
    uint32 flags[8] = { BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, BCM_L3_MC_NO_RPF, 0, 0, 0, 0 };
    uint32 rifs[8] = { vsi_loose, vsi_strict, vsi_loose, vsi_strict, vsi_loose, vsi_strict, vsi_loose, vsi_strict };
    uint32 fec_rifs[8] = { vsi_loose, vsi_strict, intf_out, intf_out, vsi_loose, vsi_strict, intf_out, intf_out };
    bcm_gport_t mc_gport_l;
    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;
    char error_msg[200]={0,};

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_start), "");

    /*
     * 1. Configure traps for failed RPF lookup - SIP-based and Explicit.
     */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv == BCM_E_PARAM) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "");

    /*
     * 2a. Set In-Port1 to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port1, vsi_loose, vrf, bcmL3IngressUrpfLoose), "");
    /*
     * 2b. Set In-Port2 to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port2, vsi_strict, vrf, bcmL3IngressUrpfStrict), "");

    /*
     * 3. Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");

    /*
     * 4. Create a multicast group and its replications
     */
    BCM_IF_ERROR_RETURN_MSG(create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports), "");

    BCM_GPORT_MCAST_SET(mc_gport_l, cint_ipmc_info.mc_group);

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }

    for (idx = 0; idx < nof_ipmc_entries; idx++)
    {
        /*
         * 5. Define expected out DMAC
         */
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, 0), "create egress object ARP only, encap_id");
        /*
         * 6. Create FECs
         */
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_start, fec_rifs[idx], 0, mc_gport_l, flags[idx], fec_flags2),
            "create egress object FEC");
        /*
         * 7. Create IPMC entry
         */
        snprintf(error_msg, sizeof(error_msg), "iter = %d", idx);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, dip_addr, dip_mask, 0, 0, rifs[idx], vrf, 0, fec_start, 0), error_msg);
        dip_addr++;
        encap_id++;
        next_hop_mac[lsb_mac]++;
        fec_start++;
    }

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        for (idx = 0; idx < nof_host_entries; idx++)
        {
            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_start, fec_rifs[4 + idx], 0, mc_gport_l, flags[4 + idx], 0), "create egress object FEC");

            BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec_start);

            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_ipmc_info.sip + idx, vrf, _l3_itf, 0, 0), "");

            fec_start++;
        }
    }

    return BCM_E_NONE;
}


/*
 * Example of an IPMC entry using a trap destination.
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/ipmc/cint_ipmc_route_examples.c
  cint
  int trap_id;
  print bcm_rx_trap_type_create(0, 0, bcmRxTrapUserDefine, &trap_id);
  ipmc_to_trap_example(0,0xCC,200, trap_id);
  exit;
 *
 * ETH-RIF packet to be sent to trap:
  tx 1 psrc=200 data=0x01005e020203000007000100810000cc0800450000350000000080009742c0800101e0020203000102030405
 *
  pp vis last
 *   No packet detected on IRPP(Ingress) for core=0
 *   =======================================================================
 *   |                      <Packet Structure Core=1>                      |
 *   |   IPv4oETH1oPTCH_2                                                  |
 *   =======================================================================
 *   |                          <Port Termination>                         |
 *   =======================================================================
 *   | PP   | IN   | Sys  | PRT                                            |
 *   | Port | TM   | Port | Qualifier                                      |
 *   |      | Port |      |                                                |
 *   =======================================================================
 *   | 0x17 | 0xc8 | 0xc8 | 0x0                                            |
 *   =======================================================================
 *   |                               <VTT Summary>                         |
 *   =======================================================================
 *   | Terminated | Global   | FODO      | FWD Layer                       |
 *   | Headers    | LIF      |           |                                 |
 *   =======================================================================
 *   | VTT1:ETH1  | 0x0000cc | VRF = 0xf | idx(01) - LAYER_TYPES_IPV4(0x2) |
 *   | VTT2:      |          |           |                                 |
 *   | VTT3:      |          |           |                                 |
 *   | VTT4:      |          |           |                                 |
 *   | VTT5:      |          |           |                                 |
 *   | FWD1:IPv4  |          |           |                                 |
 *   =======================================================================
 *   |                                <FWD Lookup>                         |
 *   =======================================================================
 *   | Key Components     | Result                                         |
 *   |                    | Destination                                    |
 *   =======================================================================
 *   | SIP = 192.128.1.1  | 0x7ffff (MC_ID)                                |
 *   | DIP = 224.2.2.3    |                                                |
 *   | VRF = 0xf          |                                                |
 *   | Global LIF =       |                                                |
 *   | 0x0000cc           |                                                |
 *   =======================================================================
 *   |                           <Trap Resolution>                         |
 *   =======================================================================
 *   | Trap Name                             | Strength                    |
 *   =======================================================================
 *   | INGRESS_TRAP_ID_USER_DEFINED_19(0xbe) | 0f                          |
 *   =======================================================================
 *   |                       <TM Command Resolution>                       |
 *   =======================================================================
 *   | Destination | Tc  | Dp                                              |
 *   =======================================================================
 *   | Not Valid   | 0x0 | 0x0                                             |
 *   =======================================================================
 * Test Scenario - end
 *
 */

/*
 * Configure interfaces, user-defined trap, routes and hosts for MC forwarding.
 * The function creates IPMC entries that result to a trap gport if LEM and TCAM tables are supported.
 * Otherwise it creates a single IPMC entry that results to a FEC that results to a trap.
 * It is expected that the packet will arrive on port in_port.
 * It is expected that if the DIP matches an entry that results to a port(trap) it will be dropped.
 *
 * It is expected that the input packet will have
 *      * DA = 01:00:5E:02:02:02
 *      * SA = 00:00:07:00:01:00
 *      * VID = rif (input)
 *      * DIP = 224.2.2.2 - 224.2.2.3
 *      * SIP = 192.128.1.1
 * IPMC entries with DIP 224.2.2.2 is saved in LEM if host entries are supported.
 * Otherwise entry with DIP 224.2.2.2 is saved in LPM.
 * IPMC entries with DIP 224.2.2.3 is saved in TCAM if TCAM table is supported.
 *
 */
int
ipmc_to_trap_example(
    int unit,
    int vid_in,
    int in_port,
    int trap_id_fec)
{
    char error_msg[200]={0,};
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

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in), "");

    /*
     * 2. Create ETH-RIF and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address), "");

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = eth_rif_id;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "ingr_itf");

    /*
     * 4. Set user defined traps and define gport to the trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_id_fec, &config), "");

    /*
     * Set maximum Trap strength to the Trap ID when creating the gport.
     */
    BCM_GPORT_TRAP_SET(trap_gport, trap_id_fec, 15, 0);
    printf("trap gport = %04x, trap_id = %04x\n", trap_gport, trap_id_fec);

    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        uint32 flags = 0;
        /*
        * 5. Add IPv4 IPMC entries
        */
        for (idx = 0; idx < nof_entries; idx++)
        {
            snprintf(error_msg, sizeof(error_msg), "idx = %d", idx);
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_to_port(unit, cint_ipmc_info.mc_dip + idx, no_mask, cint_ipmc_info.sip, 0,
                                    cint_ipmc_info.intf_in, cint_ipmc_info.vrf, trap_gport, flags), error_msg);
            flags = BCM_IPMC_TCAM;
        }
    }
    else
    {
        /** Create FEC pointing to trap ID */
        int fec_flags2 = 0;
        int fec = 0;
        BCM_IF_ERROR_RETURN_MSG(get_default_fec_id(unit, &fec), "");
        
        if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, 0, 0, trap_gport, 0, fec_flags2, 0, 0, &fec),
            "create egress object FEC only");
        /** Create IPMC entry in LPM pointing to FEC */
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, no_mask, cint_ipmc_info.sip, part_mask,
                            cint_ipmc_info.intf_in, cint_ipmc_info.vrf, 0, fec, 0), "to LPM table");
    }

    return BCM_E_NONE;
}

/*
 * IPMC with TTL filter example
 */
int
dnx_ipv4_mc_example_with_ttl_extended(
    int unit,
    int iport,
    int oport1,
    int oport2,
    int oport3)
{
    char error_msg[200]={0,};
    bcm_gport_t dst_gport;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0;
    bcm_ip_t mc_ip_2 = 0xE0E0E002; /* 224.224.224.2 */
    bcm_ip_t mc_ip_3 = 0xE0E0E003; /* 224.224.224.3 */
    int vlan = 2;
    int vrf = 2;
    int ipmc_index = 6000;
    int dest_local_port_id = iport;
    int source_local_port_id = oport1, ports[3];
    int nof_dest_ports = 3;
    int intf_out[3];
    int idx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};
    bcm_mac_t mac_l3_out[3] = {{0x00, 0x00, 0x00, 0x11, 0x11, 0x11}, {0x00, 0x00, 0x00, 0x11, 0x11, 0x22}, {0x00, 0x00, 0x00, 0x11, 0x11, 0x33}};
    l3_ingress_intf ingress_itf;
    bcm_multicast_replication_t replications[3];
    l3_ingress_intf ingr_itf;

    ports[0] = oport1;
    ports[1] = oport2;
    ports[2] = oport3;

    /** 1. Set In-Port to In ETh-RIF
     ** 2. Set Out-Port*/
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, iport, vlan), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, oport1), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, oport2), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, oport3), "");
    /** 2. Create ETH-RIF and set its properties*/
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vlan, mac_l3_ingress), "intf_in");
    /** 3. Set Incoming ETH-RIF properties*/
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vlan;
    ingr_itf.vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");
    intf_out[0] = 20;
    intf_out[1] = 21;
    intf_out[2] = 22;
    for(idx = 0; idx < nof_dest_ports; idx++)
    {   /** The ttl treshold is equal to inf_out value*/
        snprintf(error_msg, sizeof(error_msg), "intf_out=%d", intf_out[idx]);
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_with_ttl(unit, intf_out[idx], mac_l3_out[idx], intf_out[idx]), error_msg);

        set_multicast_replication(&replications[idx], ports[idx], intf_out[idx]);
    }
    BCM_IF_ERROR_RETURN_MSG(create_multicast(unit, replications, nof_dest_ports, ipmc_index), "");
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0), "");

    intf_out[0] = 30;
    intf_out[1] = 31;
    intf_out[2] = 32;
    ipmc_index++;
    for(idx = 0; idx < nof_dest_ports; idx++)
    {
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_with_ttl(unit, intf_out[idx], mac_l3_out[idx], intf_out[idx]), "intf_out");
        set_multicast_replication(&replications[idx], ports[idx], intf_out[idx]);
    }
    BCM_IF_ERROR_RETURN_MSG(create_multicast(unit, replications, nof_dest_ports, ipmc_index), "");
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, mc_ip_2, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0), "");

    ipmc_index++;
    intf_out[0] = 40;
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_with_ttl(unit, intf_out[0], mac_l3_out[0], intf_out[0]),
        "intf_out. The last intf_out create");
    set_multicast_replication(&replications[0], ports[0], intf_out[0]);

    BCM_IF_ERROR_RETURN_MSG(create_multicast(unit, replications, 1, ipmc_index), "");
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, mc_ip_3, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0), "");

    return BCM_E_NONE;
}
