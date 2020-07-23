/* $Id: cint_ip_route_rpf_scenarios.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * Test Scenario
 *     Example of a router with Ipv4 MC RPF scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_scenarios.c
 * cint
 * dvapi_run_ipv4_mc_example_rpf(0,200,201);
 * exit;
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006b85bc0800104e0e0e0013344556600a1a2a300b1b2b3500f01f57e4e0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x5e60e0010000000200028100001408004508002e000040003f06b95bc0800104e0e0e0013344556600a1a2a300b1b2b3500f01f57e4e0000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e00004000400606d178fffa0ee0e0e0023344556600a1a2a300b1b2b3500f01f5ccc30000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x5e60e0020000000200028100001408004508002e000040003f0607d178fffa0ee0e0e0023344556600a1a2a300b1b2b3500f01f5ccc30000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006fadb7fffff03e0e0e0023344556600a1a2a300b1b2b3500f01f5c0ce0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006f65a82800104e0e0e0023344556600a1a2a300b1b2b3500f01f5bc4d0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006b859c0800104e0e0e0033344556600a1a2a300b1b2b3500f01f57e4c0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006b858c0800104e0e0e0043344556600a1a2a300b1b2b3500f01f57e4b0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x5e60e0040000000200028100001408004508002e000040003f06b958c0800104e0e0e0043344556600a1a2a300b1b2b3500f01f57e4b0000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006f65782800104e0e0e0053344556600a1a2a300b1b2b3500f01f5bc4a0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port:
 *    Data: 0x5e60e0050000000200028100001408004508002e000040003f06f75782800104e0e0e0053344556600a1a2a300b1b2b3500f01f5bc4a0000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=200 data=0x01005e60e0020000000000018100000a08004508002e000040004006fad87fffff03e0e0e0053344556600a1a2a300b1b2b3500f01f5c0cb0000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x5e60e0050000000200028100001408004508002e000040003f06fbd87fffff03e0e0e0053344556600a1a2a300b1b2b3500f01f5c0cb0000f5f5f5f5f5f5f5f5f5f50000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */


int print_level = 2;
int MAX_NUM_UNIT = 8;

print_level = 2;

/* Add an IPv4 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int add_ip4mc_entry_with_egress_itf(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, int vrf, int egress_itf){
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    /* BCM_IPMC_SOURCE_PORT_NOCHECK is not supported on JR2 and above */
    int flags = (is_device_or_above(unit, JERICHO2) ? 0 : BCM_IPMC_SOURCE_PORT_NOCHECK);
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    /* Init the ipmc_addr structure and fill the requiered fields.*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    data.s_ip_addr = src_ip;
    data.vid = vlan;
    data.vrf = vrf;
    data.flags = flags;
    data.l3a_intf = egress_itf;
    if (is_device_or_above(unit,JERICHO2) & is_kbp)
    {
        /* External lookup enabled on JR2 */
        data.flags |= BCM_IPMC_TCAM;
    }
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

/* Used for dvapi only*/
int dvapi_run_ipv4_mc_example_rpf(int unit, int in_port, int out_port)
{
    ipv4_mc_example_rpf(unit,in_port,out_port,1);
}

/* Main IPv4 MC Example */
int ipv4_mc_example_rpf(int unit, int in_port, int out_port,int nof_dest_ports)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0x0; /* masked */
    int ipmc_index = 6000;

    int flags = 0;
    bcm_gport_t mc_gport_r;
    int idx;

    int fec[10] = {0x0};
    int eg_itf_index = 0;

    int encap_id_r = 0;
    int encap_id_l = 0;
    int vrf = 1;

    int host_l = 0x78ffff03; /*120.255.255.0*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;
    int ipmc_index_l;
    bcm_gport_t mc_gport_l;

    int host_r = 0x7fffff03; /*128.255.255.3*/
    int subnet_r = 0x7ffffa00; /*128.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    bcm_multicast_replication_t replications[1];
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    int vlan;
    l3_ingress_intf ingr_itf;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;

    int system_headers_mode = soc_property_get(unit , "system_headers_mode", 1);

    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;
    uint32 l3rt_rpf_flags2 = 0;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        /* External lookup enabled on JR2 */
        l3rt_rpf_flags2 = BCM_L3_FLAGS2_RPF_ONLY;
    }

    l3_ingress_intf_init(&ingr_itf);

    /* 1. Configure traps for failed RPF lookup - SIP-based and Explicit. */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

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

    /* 3. Set ingress interface */
    ingr_itf.intf_id = ing_intf_l;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /* 5. Create the IP MC Group */
    ipmc_index_l = ipmc_index+1;
    bcm_multicast_destroy(unit,ipmc_index);
    bcm_multicast_destroy(unit,ipmc_index_l);

    /* multicast for R-ports */
    flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID) :
        (BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
    rv = bcm_multicast_create(unit, flags, &ipmc_index);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    uint32 dummy_flags;
    int nof_replications = 1;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int out_ports[1] = { out_port };

    bcm_module_t mreps[2] = {-1, -1};
    int nof_mreps = 0;
    for (idx = 0; idx < nof_replications; idx ++)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            rv = bcm_port_get(unit, out_ports[idx], &dummy_flags, &interface_info, &mapping_info);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_get failed $rv\n");
                return rv;
            }
            /** On single device core ID and module ID match */
            if (mreps[0] == -1)
            {
                mreps[0] = mapping_info.core;
                nof_mreps = 1;
            }
            else if (mreps[0] != mapping_info.core)
            {
                mreps[1] = mapping_info.core;
                nof_mreps = 2;
            }
        }
        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_EGRESS_GROUP : 0;
        bcm_multicast_replication_t_init(&replications[idx]);
        replications[idx].encap1 = vlan_r;
        replications[idx].port = out_ports[idx];
    }
    rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, ipmc_index, ipmc_index);
    if (rv != BCM_E_NONE) {
       printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
       return rv;
    }
    rv = bcm_multicast_set(unit, ipmc_index, flags, nof_replications, &replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_add\n");
        return rv;
    }
    BCM_GPORT_MCAST_SET(mc_gport_r,ipmc_index);


    /* multicast for L-ports */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ipmc_index_l);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP: BCM_MULTICAST_INGRESS;
    bcm_multicast_replication_t_init(&replications);
    replications.encap1 = vlan_l;
    replications.port = in_port;
    rv = bcm_multicast_set(unit, ipmc_index_l, flags, 1, &replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_add\n");
        return rv;
    }
    BCM_GPORT_MCAST_SET(mc_gport_l,ipmc_index_l);

    /*
     * create egress object with different RPF checks
     */

    /*
                L3 flags    |      Device-config
     ----------------------------------------------
            BCM_L3_RPF  IPMC    |   MC-RPF-Mode
            ---------   ----    |  -----------
                V       X       |   SIP-mode
                X       X       |   No check
                V       V       |   Explicit
                                |   intf used as expected RIF
     ----------------------------------------------
     */

    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = mac_address_next_hop_r;
    l3eg.encap_id = encap_id_r;
    l3eg.vlan = 0;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, \n");
        return rv;
    }
    encap_id_r = l3eg.encap_id;

    l3eg.mac_addr = mac_address_next_hop_l;
    l3eg.encap_id = encap_id_l;
    l3eg.vlan = 0;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, \n");
        return rv;
    }
    encap_id_l = l3eg.encap_id;

    /*
     *  1. no RPF check
     */
    bcm_l3_egress_t_init(&l3eg);
    /* In JR mode, outlif pointer as EEI */
    if (is_device_or_above(unit, JERICHO2) && (system_headers_mode == 0)){
        l3eg.flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    l3eg.intf = ing_intf_r;
    l3eg.encap_id = encap_id_r;
    l3eg.port = mc_gport_r;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x with no RPF check  \n", fec[eg_itf_index]);
    }
    ++eg_itf_index;

    /*
     * 2. use SIP RPF check
      */

    /* create egress objects for IPMC with RPF use SIP strict */
    l3eg.flags = BCM_L3_MC_RPF_SIP_BASE;
    l3eg.intf = ing_intf_r;
    l3eg.encap_id = 0;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x with RPF use SIP \n", fec[eg_itf_index]);
    }
    ++eg_itf_index;


    /*
     * 3. use explicit RPF check
     */

    /* create egress objects for IPMC with RPF incorrect expected inRIF */
    l3eg.flags = BCM_L3_MC_RPF_EXPLICIT;
    l3eg.intf = ing_intf_r;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_r);
    }
    ++eg_itf_index;


    /* create egress objects for IPMC with RPF correct expected inRIF */
    l3eg.intf = ing_intf_l;
    l3eg.encap_id = 0;
    l3eg.flags = BCM_L3_MC_RPF_EXPLICIT;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_l);
    }
    ++eg_itf_index;

    vlan = (is_device_or_above(unit, JERICHO2)) ? vlan_l: 0;
    for(idx = 0; idx < eg_itf_index; ++idx) {
        /* Create the IP MC entry */
        rv = add_ip4mc_entry_with_egress_itf(unit, mc_ip, src_ip, vlan, vrf, fec[idx]);
        if (rv != BCM_E_NONE) return rv;
        mc_ip++;

    }
    /* add mc-entry points directly to MC group: bypass RPF  check */
    rv = add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0x0, vlan, vrf, ipmc_index, 0, 0);
    if (rv != BCM_E_NONE) return rv;
    mc_ip++;

    /* add IPuc route/hosts used for SIP lookup */

    /* create egress objects for IPUC (SIP) with incorrect inRIF */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.intf = ing_intf_r;
    l3eg.encap_id = encap_id_r;
    l3eg.port = out_port;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_r);
    }


    /* add host points to egress interface with incorrect RIF */
    rv = add_host_ipv4(unit, host_r, vrf, fec[eg_itf_index], 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, add host = 0x%08x, \n", host_r);
    }
    if(print_level >= 1) {
        print_host("add host ", host_r,vrf);
        printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[eg_itf_index], ing_intf_r, out_port);
    }

    ++eg_itf_index;

    /* create egress objects for IPUC (SIP) with correct inRIF */
    bcm_l3_egress_t_init(&l3eg);

    l3eg.intf = ing_intf_l;
    l3eg.encap_id = encap_id_l;
    l3eg.port = in_port;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fec[eg_itf_index]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rv;
    }

    if(print_level >= 2) {
        printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_l);
    }
    /* add route points to egress interface with correct RIF */
    sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, subnet_l, subnet_l_mask, vrf, 0, l3rt_rpf_flags2, fec[eg_itf_index]);
    rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
        return rv;
    }

    ++eg_itf_index;


    return rv;
}

int
example_rpf_ipv6(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport,
    bcm_l3_ingress_urpf_mode_t urpf_mode)
{
    int rv, i;
    int unit, flags;
    int fec[2] = {0x0};
    bcm_if_t encoded_fec[MAX_NUM_UNIT][2] = {{0}};
    int itf_flags = BCM_L3_RPF;
    int egr_flags = 0;
    int vrf = 10;
    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    sand_utils_l3_host_ipv6_s  host_ipv6_structure;
    sand_utils_l3_route_ipv6_s route_ipv6_structure;
    uint32 fwd_only_flags = 0;
    uint32 rpf_only_flags = 0;

    bcm_ip6_t host_l; /*(0100:1600:7800:8800:0000:db07:1200:0000)*/
    bcm_ip6_t mask_l;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;

    bcm_ip6_t host_r; /*(0100:1600:7800:8800:0000:db07:1200:0000)*/
    bcm_ip6_t mask_r;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    int encap_id[2]={0x2002, 0x2004}; /* EncapID use the valid global Out-LIF for both JR and JR2 */

    int sysport_is_local = 0;
    int local_port = 0;

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        fwd_only_flags = BCM_L3_FLAGS2_FWD_ONLY;
        rpf_only_flags = BCM_L3_FLAGS2_RPF_ONLY;
    }

    /* Configure traps for UC RPF - Loose and Strict */
    rv = l3_dnx_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
    }


    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, vlan_l);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", vlan_l, unit);
        }

        /* Set VLAN with MC */
        rv = bcm_vlan_control_vlan_get(unit, vlan_l, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", vlan_l);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        rv = bcm_vlan_control_vlan_set(unit, vlan_l, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", vlan_l);
            return rv;
        }

        sysport_is_local = 0;
        rv = system_port_is_local(unit, in_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
                return rv;
            }
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan_l, flags, 0, mac_address_l, vrf);
        eth_rif_structure.urpf_mode = urpf_mode;
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_l = eth_rif_structure.l3_rif;
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, vlan_r);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", vlan_r, unit);
        }

        /* Set VLAN with MC */
        rv = bcm_vlan_control_vlan_get(unit, vlan_r, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", vlan_r);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        rv = bcm_vlan_control_vlan_set(unit, vlan_r, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", vlan_r);
            return rv;
        }

        sysport_is_local = 0;
        rv = system_port_is_local(unit, out_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
              return rv;
            }
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan_r, flags, 0, mac_address_r, vrf);
        eth_rif_structure.urpf_mode = urpf_mode;
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_r = eth_rif_structure.l3_rif;
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[0], 0, 0, mac_address_next_hop_r, vlan_r);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create: encap_id = %d, vsi = %d, err_id = %d\n",
                   encap_id[0], vlan_r, rv);
        }
        encap_id[0] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        l3_fec.destination = out_sysport;
        l3_fec.tunnel_gport = ing_intf_r;
        l3_fec.tunnel_gport2 = l3_arp.l3eg_arp_id;
        l3_fec.fec_id = fec[0];
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
                 fec[0], ing_intf_r, l3_arp.l3eg_arp_id, out_sysport, rv);
            return rv;
        }
        encoded_fec[unit][0] = l3_fec.l3eg_fec_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", encoded_fec[unit][0]);
            printf("next hop mac at encap-id %08x, \n", l3_arp.l3eg_arp_id);
        }
    }


    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[1], 0, 0, mac_address_next_hop_l, vlan_l);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create: encap_id = %d, vsi = %d, err_id = %d\n",
                   encap_id[1], vlan_l, rv);
        }
        encap_id[1] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
        l3_fec.destination = in_sysport;
        l3_fec.tunnel_gport = ing_intf_l;
        l3_fec.tunnel_gport2 = l3_arp.l3eg_arp_id;
        l3_fec.fec_id = fec[1];
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
                 fec[1], ing_intf_l, l3_arp.l3eg_arp_id, in_sysport, rv);
            return rv;
        }
        encoded_fec[unit][1] = l3_fec.l3eg_fec_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", encoded_fec[unit][1]);
            printf("next hop mac at encap-id %08x, \n", l3_arp.l3eg_arp_id);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /* Add SIP Routing Entry / IN-RIF / FEC and OUT-RIF  */
    /* SIP ==    0100:0100:0000:0000:0000:0000:0000:0123 */
    /* UC IPV6 SIP: */
    host_l[15]= 0x23; /* LSB */
    host_l[14]= 0x01;
    host_l[13]= 0x00;
    host_l[12]= 0x00;
    host_l[11]= 0x00;
    host_l[10]= 0x00;
    host_l[9] = 0x00;
    host_l[8] = 0x00;
    host_l[7] = 0x00;
    host_l[6] = 0x00;
    host_l[5] = 0x00;
    host_l[4] = 0x00;
    host_l[3] = 0x00;
    host_l[2] = 0x01;
    host_l[1] = 0x00;
    host_l[0] = 0x01; /* MSB */

    /* DIP ==    0200:0200:0000:0000:0000:0000:0000:0135 */
    /* UC IPV6 DIP: */
    host_r[15]= 0x35; /* LSB */
    host_r[14]= 0x01;
    host_r[13]= 0x00;
    host_r[12]= 0x00;
    host_r[11]= 0x00;
    host_r[10]= 0x00;
    host_r[9] = 0x00;
    host_r[8] = 0x00;
    host_r[7] = 0x00;
    host_r[6] = 0x00;
    host_r[5] = 0x00;
    host_r[4] = 0x00;
    host_r[3] = 0x00;
    host_r[2] = 0x02;
    host_r[1] = 0x00;
    host_r[0] = 0x02; /* MSB */

    /* UC IPV6 SIP MASK: */
    mask_l[15]= 0x00;
    mask_l[14]= 0x00;
    mask_l[13]= 0x00;
    mask_l[12]= 0x00;
    mask_l[11]= 0x00;
    mask_l[10]= 0x00;
    mask_l[9] = 0x00;
    mask_l[8] = 0x00;
    mask_l[7] = 0xff;
    mask_l[6] = 0xff;
    mask_l[5] = 0xff;
    mask_l[4] = 0xff;
    mask_l[3] = 0xff;
    mask_l[2] = 0xff;
    mask_l[1] = 0xff;
    mask_l[0] = 0xff;

    /* UC IPV6 SIP MASK: */
    mask_r[15]= 0x00;
    mask_r[14]= 0x00;
    mask_r[13]= 0x00;
    mask_r[12]= 0x00;
    mask_r[11]= 0x00;
    mask_r[10]= 0x00;
    mask_r[9] = 0x00;
    mask_r[8] = 0x00;
    mask_r[7] = 0xff;
    mask_r[6] = 0xff;
    mask_r[5] = 0xff;
    mask_r[4] = 0xff;
    mask_r[3] = 0xff;
    mask_r[2] = 0xff;
    mask_r[1] = 0xff;
    mask_r[0] = 0xff;

    /* IPuc add routes/host to Routing table: from L to R*/
    {
        /*** add host point to FEC2 ***/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];

            sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, host_r, vrf, 0, 0, encoded_fec[unit][0]);
            rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in function sand_utils_l3_host_ipv6_add, \n");
                return rv;
            }

            /* If the scale feature is enabled, also add the entries to the special KAPS tables */
            if (is_device_or_above(unit, JERICHO) && !is_device_or_above(unit, JERICHO2) &&
                ((soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_LONG, 0)) ||
                 (soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_SHORT, 0)))) {
                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_r, mask_r, vrf, 0, BCM_L3_FLAGS2_SCALE_ROUTE, encoded_fec[unit][0]);
                rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                    return rv;
                }
            }

            if(verbose >= 1) {
                ipv6_print(host_r);
                printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, vrf=%d\n", encoded_fec[unit][0], ing_intf_r, in_sysport, vrf);
            }
        }

        /*** add route point to FEC2 ***/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_r, mask_r, vrf, 0, fwd_only_flags, encoded_fec[unit][0]);
            rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                return rv;
            }
            if(verbose >= 1) {
                ipv6_print(host_r);printf("/");ipv6_print(mask_r);
                printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][0], ing_intf_r, out_sysport);
            }
            if (rpf_only_flags) {
                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_r, mask_r, vrf, 0, rpf_only_flags, encoded_fec[unit][0]);
                rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                    return rv;
                }
                if(verbose >= 1) {
                    ipv6_print(host_r);printf("/");ipv6_print(mask_r);
                    printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][0], ing_intf_r, out_sysport);
                }
            }
        }

        /* IPuc add routes/host to Routing table: from R to L*/
        /*** add host point to FEC2 ***/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];

            sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, host_l, vrf, 0, 0, encoded_fec[unit][1]);
            rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in function sand_utils_l3_host_ipv6_add, \n");
                return rv;
            }

            /* If the scale feature is enabled, also add the entries to the special KAPS tables */
            if (is_device_or_above(unit, JERICHO) && !is_device_or_above(unit, JERICHO2) &&
                ((soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_LONG, 0)) ||
                 (soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_SHORT, 0)))) {
                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_l, mask_l, vrf, 0, BCM_L3_FLAGS2_SCALE_ROUTE, encoded_fec[unit][1]);
                rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                    return rv;
                }
            }

            if(verbose >= 1) {
                ipv6_print(host_l);
                printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, vrf=%d\n", encoded_fec[unit][1], ing_intf_l, in_sysport, vrf);
            }
        }

        /*** add route point to FEC2 ***/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_l, mask_l, vrf, 0, fwd_only_flags, encoded_fec[unit][1]);
            rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                return rv;
            }

            if(verbose >= 1) {
                ipv6_print(host_l);printf("/");ipv6_print(mask_l);
                printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][1], ing_intf_l, in_sysport);
            }
            if (rpf_only_flags) {
                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, host_l, mask_l, vrf, 0, rpf_only_flags, encoded_fec[unit][1]);
                rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function sand_utils_l3_route_ipv6_add, \n");
                    return rv;
                }

                if(verbose >= 1) {
                    ipv6_print(host_l);printf("/");ipv6_print(mask_l);
                    printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][1], ing_intf_l, in_sysport);
                }
            }
        }
    }
    return rv;
}

int L3_rpf_default_fec_enable = 1;
int L3_rpf_fec_cascade = 0;

/*
 * Test Scenario
 *   test Ipv4 uRPF basic function
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_scenarios.c
 * cint
 * example_rpf_vrf(units_ids,1,1811939528,1811939531,bcmL3IngressUrpfLoose,1);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400639bf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400641517ffffa6e7fffff033344556600a1a2a300b1b2b3500f01f507440000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100001408004508002e000040003f0642517ffffa6e7fffff033344556600a1a2a300b1b2b3500f01f507440000f5f5f5f5f5f5f5f5f5f50000
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e000040004006485178fffa6e7fffff033344556600a1a2a300b1b2b3500f01f50e440000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100001408004508002e000040003f06495178fffa6e7fffff033344556600a1a2a300b1b2b3500f01f50e440000f5f5f5f5f5f5f5f5f5f50000
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400639bf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100001408004508002e000040003f063abf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f50000
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_scenarios.c
 * cint
 * example_rpf_vrf(units_ids,1,1811939528,1811939531,bcmL3IngressUrpfStrict,1);
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400639bf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400641517ffffa6e7fffff033344556600a1a2a300b1b2b3500f01f507440000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e000040004006485178fffa6e7fffff033344556600a1a2a300b1b2b3500f01f50e440000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100001408004508002e000040003f06495178fffa6e7fffff033344556600a1a2a300b1b2b3500f01f50e440000f5f5f5f5f5f5f5f5f5f50000
 *
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400639bf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100001408004508002e000040003f063abf820000007fffff033344556600a1a2a300b1b2b3500f01f5ffb10000f5f5f5f5f5f5f5f5f5f50000
 */
int example_rpf_vrf(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode, int vrf){
    int rv, i;
    int unit, flags;
    bcm_if_t encoded_fec[MAX_NUM_UNIT][2] = {{0}};
    int encap_id[2]={0x2000, 0x2002};
    int itf_flags = BCM_L3_RPF;
    int ingress_flags[MAX_NUM_UNIT] = {0};
    int egr_flags[MAX_NUM_UNIT] = {0};
    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s l3_arp;
    sand_utils_l3_fec_s l3_fec;
    bcm_l3_host_t l3host;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;
    uint32 l3rt_rpf_flags2 = 0;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        /* External lookup enabled on JR2 */
        l3rt_rpf_flags2 = BCM_L3_FLAGS2_RPF_ONLY;
    }

    int host_l = 0x78ffff03; /*120.255.255.3*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;

    int host_r = 0x7fffff03; /*127.255.255.3*/
    int subnet_r = 0x7ffffa00; /*127.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    int sysport_is_local = 0;
    int local_port = 0;

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        if ((vrf == 0) && is_device_or_above(unit,JERICHO))
        {
            ingress_flags[unit] = BCM_L3_INGRESS_GLOBAL_ROUTE;
        }

        if (is_device_or_above(unit, JERICHO2)){
            continue;
        }

        /* flag BCM_L3_CASCADED is not used in JR2 */
        if (L3_rpf_fec_cascade) {
            egr_flags[unit] |= BCM_L3_CASCADED;
            printf("FEC's will be cascaded\n");
        }

       /* For JR1, only one global MSBs My-MAC was supported. bcm_l2_station_add API is used to assign a global MSBs my-MAC to a VSI.
        * For JR2, there is no need to call bcm_l2_station_add. VSI global MSBs my-MAC assignment is managed internally using the API bcm_l3_intf_create.
         */
        int station_id;
        bcm_l2_station_t station;
        bcm_l2_station_t_init(&station);
        /* set my-Mac global MSB */
        station.flags = 0;
        sal_memcpy(station.dst_mac, mac_address_l, 6);
        station.src_port_mask = 0; /* port is not valid */
        station.vlan_mask = 0; /* vsi is not valid */
        station.dst_mac_mask[0] = station.dst_mac_mask[1] = station.dst_mac_mask[2] =
        station.dst_mac_mask[3] = station.dst_mac_mask[4] = station.dst_mac_mask[5] = 0xff; /* dst_mac my-Mac MSB mask is -1 */

        rv = bcm_l2_station_add(unit, &station_id, &station);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_station_add\n");
            return rv;
        }
    }

    /* Configure traps for UC RPF - Loose and Strict */
    rv = l3_dnx_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
    }

    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, vlan_l);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", vlan_l, unit);
        }

        rv = bcm_vlan_control_vlan_get(unit, vlan_l, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", vlan_l);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        control_vlan.entropy_id = 0;
        rv = bcm_vlan_control_vlan_set(unit, vlan_l, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", vlan_l);
            return rv;
        }

        sysport_is_local = 0;
        rv = system_port_is_local(unit, in_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
              return rv;
            }
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan_l, flags, ingress_flags[unit], mac_address_l, vrf);
        eth_rif_structure.urpf_mode = urpf_mode;
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_l = eth_rif_structure.l3_rif;
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_vlan_create(unit, vlan_r);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, create vlan=%d, in unit %d \n", vlan_r, unit);
        }

        rv = bcm_vlan_control_vlan_get(unit, vlan_r, &control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail get control vlan(%d)\n", vlan_r);
            return rv;
        }
        control_vlan.unknown_unicast_group =
            control_vlan.unknown_multicast_group =
            control_vlan.broadcast_group = 0x1;
        control_vlan.entropy_id = 0;
        rv = bcm_vlan_control_vlan_set(unit, vlan_r, control_vlan);
        if (rv != BCM_E_NONE) {
            printf("fail set control vlan(%d)\n", vlan_r);
            return rv;
        }

        sysport_is_local = 0;
        rv = system_port_is_local(unit, out_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
                return rv;
            }
        }

        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan_r, flags, ingress_flags[unit], mac_address_r, vrf);
        eth_rif_structure.urpf_mode = urpf_mode;
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
            return rv;
        }
        ing_intf_r = eth_rif_structure.l3_rif;
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        flags = egr_flags[unit];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[0], 0, 0, mac_address_next_hop_r, vlan_r);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }
        encap_id[0] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_common_init(unit, flags, 0, &l3_fec, encoded_fec[unit][0], 0, 0, out_sysport, ing_intf_r, encap_id[0]);
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        encoded_fec[unit][0] = l3_fec.l3eg_fec_id;
        if(verbose >= 1) {
            printf("On unit %d: created FEC-id =0x%08x, \n", unit, encoded_fec[unit][0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
    }

    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        flags = egr_flags[unit];

        /* Create ARP and set its properties */
        sand_utils_l3_arp_s_common_init(unit, 0, &l3_arp, encap_id[1], 0, 0, mac_address_next_hop_l, vlan_l);
        rv = sand_utils_l3_arp_create(unit, &l3_arp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }
        encap_id[1] = l3_arp.l3eg_arp_id;

        /* Create FEC and set its properties */
        sand_utils_l3_fec_s_common_init(unit, flags, 0, &l3_fec, encoded_fec[unit][1], 0, 0, in_sysport, ing_intf_l, encap_id[1]);
        rv = sand_utils_l3_fec_create(unit, &l3_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        encoded_fec[unit][1] = l3_fec.l3eg_fec_id;
        if(verbose >= 1) {
            printf("On unit %d: created FEC-id =0x%08x, \n", unit, encoded_fec[unit][1]);
            printf("next hop mac at encap-id %08x, \n", encap_id[1]);
        }
    }


    /* IPuc add routes/host to Routing table: from L to R*/
    /*** add host point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        bcm_l3_host_t_init(l3host);
        l3host.l3a_ip_addr = host_r;
        l3host.l3a_vrf = vrf;
        l3host.l3a_intf = encoded_fec[unit][0];     /* dest is FEC + OutLif */
        rv = bcm_l3_host_add(unit, l3host);
        if (rv != BCM_E_NONE) {
            printf ("bcm_l3_host_add failed: %d\n", rv);
        }
        if(verbose >= 1) {
            print_host("add host ", host_r,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d\n", encoded_fec[unit][0], ing_intf_r, out_sysport);
        }
    }

    /*** add route point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, subnet_r, subnet_r_mask, vrf, 0, 0, encoded_fec[unit][0]);
        rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
            return rv;
        }

        if(verbose >= 1) {
            print_route("add subnet ", subnet_r,subnet_r_mask,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][0], ing_intf_r, out_sysport);
        }
    }

   /* IPuc add routes/host to Routing table: from R to L*/
   /*** add host point to FEC2 ***/
   for (i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];

        bcm_l3_host_t_init(l3host);
        l3host.l3a_ip_addr = host_l;
        l3host.l3a_vrf = vrf;
        l3host.l3a_intf = encoded_fec[unit][1];     /* dest is FEC + OutLif */
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2. Use LEM entry for RPF check */
            l3host.l3a_flags = BCM_L3_HOST_LOCAL;
        }
        rv = bcm_l3_host_add(unit, l3host);
        if (rv != BCM_E_NONE) {
            printf ("bcm_l3_host_add failed: %d\n", rv);
        }
        if(verbose >= 1) {
            print_host("add host ", host_l,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d\n", encoded_fec[unit][1], ing_intf_l, in_sysport);
        }
   }

   /*** add route point to FEC2 ***/
   for (i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, subnet_l, subnet_l_mask, vrf, 0, l3rt_rpf_flags2, encoded_fec[unit][1]);
      rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
      if (rv != BCM_E_NONE)
      {
          printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
          return rv;
      }

      if(verbose >= 1) {
          print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
          printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][1], ing_intf_l, in_sysport);
      }

      if (L3_rpf_default_fec_enable) {
          /* Add default route for bcmSwitchL3UrpfDefaultRoute */
          sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, 0, 0, vrf, 0, l3rt_rpf_flags2, encoded_fec[unit][1]);
          rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
              return rv;
          }

          if(verbose >= 1) {
              print_route("add subnet ", 0,0,vrf);
              printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", encoded_fec[unit][1], ing_intf_l, in_sysport);
          }
      }
    }
    return rv;
}

/*
 * Test Scenario
 *     Example for RPF where the mode is per RIF
 *
 * Details:
 *   example_rpf is first called to do the same setup (see example_rpf).
 *   This will cause both RIFs (10 and 20) to be setup and configured strict uRPF.
 *   Next, two LIFs are created (LIFs are required for uRPF mode),
 *   and two different uRPF modes are configured for each RIF:
 *   10 - Configured to perform loose uRPF.
 *   20 - Configured to perform strict uRPF.
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/sand/cint_ip_route_rpf_scenarios.c
 * cint
 * example_rpf_per_rif_urpf_mode(0,200,203);
 * exit;
 *
 *    STAGE1: Loose - sip not found
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400639bf820000007fffff033344556600a1a2a300b1b2b3500001f5ffc00000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 *    STAGE2: Loose - wrong rif
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e00004000400641517ffffa6e7fffff033344556600a1a2a300b1b2b3500001f507530000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100801408004508002e000040003f0642517ffffa6e7fffff033344556600a1a2a300b1b2b3500001f507530000f5f5f5f5f5f5f5f5f5f50000
 *
 *    STAGE3: Loose - correct rif
 * tx 1 psrc=200 data=0x0000000200010000070001008100000a08004508002e000040004006485178fffa6e7fffff033344556600a1a2a300b1b2b3500001f50e530000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1e0000000200028100801408004508002e000040003f06495178fffa6e7fffff033344556600a1a2a300b1b2b3500001f50e530000f5f5f5f5f5f5f5f5f5f50000
 *
 *    STAGE1: Strict - sip not found
 * tx 1 psrc=200 data=0x0000000200020000070001008100001408004508002e00004000400640bf8200000078ffff033344556600a1a2a300b1b2b3500001f506c10000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 *    STAGE2: Strict - wrong rif
 * tx 1 psrc=200 data=0x0000000200020000070001008100001408004508002e0000400040064f5178fffa6e78ffff033344556600a1a2a300b1b2b3500001f515530000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *
 *    STAGE3: Strict - correct rif
 * tx 1 psrc=200 data=0x0000000200020000070001008100001408004508002e00004000400648517ffffa6e78ffff033344556600a1a2a300b1b2b3500001f50e530000f5f5f5f5f5f5f5f5f5f5
 *
 *    Received packets on unit 0 should be:
 *    Source port: 0, Destination port: 0
 *    Data: 0x0000cd1d0000000200018100800a08004508002e000040003f0649517ffffa6e78ffff033344556600a1a2a300b1b2b3500001f50e530000f5f5f5f5f5f5f5f5f5f50000
 */
int
example_rpf_per_rif_urpf_mode (
    int unit,
    int in_sysport,
    int out_sysport)
{
    int rv = BCM_E_NONE;
    int is_arad_plus = 0;
    int vlan_l = 10;
    int vlan_r = 20;
    int i;
    int units_ids[] = {unit};
    bcm_gport_t lif;
    int vlan_arr[] = {vlan_l, vlan_r};
    int vlan_port_arr[] = {in_sysport, out_sysport};
    int vlan_n = 2;
    bcm_l3_ingress_t l3i;
    bcm_vlan_port_t vp;

    /* Make sure the device is Arad+ as uRPF per RIF is supported only on Arad+ and above. */
    if (!is_device_or_above(unit,ARAD_PLUS)) {
        printf("Error - uRPF mode per RIF is only supported for Arad+.\n");
        return BCM_E_FAIL;
    }

    rv = is_arad_plus_only(unit, &is_arad_plus);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_arad_plus_only: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }

    /* Initial setup - setup RIF 10, 20 with the same configuration as the global rpf example. */
    /* Initially both RIFs are configured to perform strict uRPF. */
    rv = example_rpf_vrf(units_ids, 1, in_sysport, out_sysport, bcmL3IngressUrpfStrict, 1);
    if (rv != BCM_E_NONE) {
        printf("Error doing example_rpf.\n");
        print rv;
        return rv;
    }

    /* Create a LIF for each RIF. This is important for the uRPF mode to work per RIF. */
    if (is_arad_plus) {
        for (i = 0; i < vlan_n; i++) {
            /* Create a lif for <vlan,port>. */
            bcm_vlan_port_t_init(&vp);
            vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vp.port = vlan_port_arr[i];
            vp.match_vlan = vlan_arr[i];
            vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vlan_arr[i]; /* when forwarded to this port, packet will be set with this out-vlan */
            vp.vsi = vlan_arr[i];
            vp.flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
            rv = bcm_vlan_port_create(unit, &vp);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_create\n");
                return rv;
            }
            lif = vp.vlan_port_id;
        }
    }

    rv = out_port_set(unit, out_sysport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_sysport\n");
        return rv;
    }

    /* Change the uRPF mode of RIF 10 to loose. */
    /* Get the configuration of the InRIF. */
    rv = bcm_l3_ingress_get(unit, vlan_l, l3i);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n");
        printf("bcm_l3_ingress_get(%d, %d, l3i)\n", unit, vlan_l);
        print l3i;
        print rv;
        return rv;
    }

    /* Change the RPF mode. */
    l3i.urpf_mode = bcmL3IngressUrpfLoose;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3i, vlan_l);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n");
        printf("bcm_l3_ingress_create(%d, l3i, %d)\n", unit, vlan_l);
        print l3i;
        print rv;
        return rv;
    }

    return rv;
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
    int sip_count = 2;
    int i, j, found, l3egid, rc, rv, flags, vlan;
    uint8 mac_last_byte[NOF_BASIC_MC_GROUP];
    int exPorts[4] = {port0 , port1, port2 , port3};
    int ipmc_group[NOF_BASIC_MC_GROUP];
    int out_ports[NOF_BASIC_MC_GROUP] = {3, 3, 2, 2};
    bcm_multicast_replication_t replications[NOF_BASIC_MC_GROUP];
    bcm_l3_egress_t l3_eg;
    bcm_if_t l3egid_null;
    l3_ingress_intf ingr_itf;
    int fec[5];
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    if (is_device_or_above(unit, JERICHO2)) {
        ipmc_index_offset = 0;
        ipmc_index_upper_bound = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));
    } else {
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmap_groups");
        ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    }

    /*Different last byte of the MAC address for the interfaces*/
    mac_last_byte[0] = 0x10;
    for (i = 1; i < NOF_BASIC_MC_GROUP; i++) {
        mac_last_byte[i] = mac_last_byte[i - 1] + 0x20;
    }

    /*Create traps to catch the explicit MC RPF and SIP MC RPF fails*/
    rc = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rc != BCM_E_NONE)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rc;
    }

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

        rv = bcm_vlan_gport_add(unit, vlan, port0, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_gport_add - port(0x%08x) to vlan(%d)\n", port0, vlan);
            return rv;
        }

        rv = intf_eth_rif_create(unit, vlan, mac_l3_ingress_add);
        if (rv != BCM_E_NONE)
        {
             printf("Error, intf_eth_rif_create intf_in\n");
             return rv;
        }

        ingr_itf.intf_id = ingr_itf.vrf = vlan;
        rv = intf_ingress_rif_set(unit, &ingr_itf);
        if (rv != BCM_E_NONE)
        {
             printf("Error, intf_eth_rif_create intf_out\n");
             return rv;
        }

        flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID) : (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
        /*Create MC groups*/
        rc = bcm_multicast_create(unit, flags, &(ipmc_group[i]));
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rc;
        }

        /*Add ports to each MC group*/
        for (j = 0; j < out_ports[i]; j++) {
            bcm_multicast_replication_t_init(&replications[j]);
            replications[j].port = exPorts[(j + 1) % 4];
            replications[j].encap1 = vlan;
        }

        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
        rc = bcm_multicast_set(unit, ipmc_group[i], flags, out_ports[i], replications);
        if (rc != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_set\n");
            return rc;
        }
    }

    bcm_ipmc_addr_t data;

    bcm_ip6_t route = {0xFF,0xFF,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,0,0,0};
    bcm_ip6_t supported_sip  = {0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16};
    bcm_ip6_t kbp_fwd_sip  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x23};
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
        rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3_eg, &l3egid_null);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object, \n");
            return rc;
        }

        if (i < explicit_count) {
            /*Set the first entry with an incorrect in-rif */
            l3_eg.intf = vlan + (1 - i) * 5;
            l3_eg.flags = BCM_L3_MC_RPF_EXPLICIT;
        } else {
            l3_eg.flags = BCM_L3_MC_RPF_SIP_BASE;
        }

        BCM_GPORT_MCAST_SET(l3_eg.port, ipmc_group[i]);
        l3_eg.encap_id = 0;
        rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_eg, &fec[i]);
        if (rc != BCM_E_NONE)
        {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rc;
        }

        frwdl3egid = fec[i];

        bcm_ipmc_addr_t_init(&data);
        sal_memcpy(data.mc_ip6_addr, route, 16);
        sal_memcpy(data.mc_ip6_mask, full_mask, 16);
        if (i == explicit_count + sip_count -1) {
            sal_memcpy(data.s_ip6_addr, supported_sip, 16);
        } else {
            sal_memcpy(data.s_ip6_addr, kbp_fwd_sip, 16);
        }
        data.flags = BCM_IPMC_IP6;
        data.vid = data.vrf = vlan;
        data.group = 0;
        data.l3a_intf = frwdl3egid;
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2 */
            data.flags |= BCM_IPMC_TCAM;
        }

        printf("frwdl3egid 0x%x \n",frwdl3egid);
        rc = bcm_ipmc_add(unit,&data);

        if (rc != BCM_E_NONE) {
             printf("Error, create_forwarding_entry_dip_sip \n");
             return rc;
        }
        route[15] += 0x10;
    }

    mac_l3_engress_add[5] = 11;
    bcm_l3_egress_t_init(&l3_eg);
    sal_memcpy(l3_eg.mac_addr, mac_l3_engress_add, 6);
    rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3_eg, &l3egid_null);
    if (rc != BCM_E_NONE)
    {
        printf("Error, create egress object, \n");
        return rc;
    }

    /*Creating a FEC to have a payload for the SIP check (won't be used for actual forwarding)*/
    l3_eg.flags = BCM_L3_MC_RPF_SIP_BASE;
    l3_eg.intf = vlan_bais_in + (3 * vlan_offset_in);
    l3_eg.encap_id = 0;
    l3_eg.port = port1;
    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_eg, &fec[4]);
    if (rc != BCM_E_NONE)
    {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

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
    rc = bcm_l3_route_add(unit, &l3route);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %d \n", rc);
        return rc;
    }

    return rc;
}

