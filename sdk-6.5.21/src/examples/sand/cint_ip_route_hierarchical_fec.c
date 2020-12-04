/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ip_route_rpf_basic.c
 * cint src/examples/sand/cint_ip_route_hierarchical_fec.c
 * cint
 * dnx_example_ipv4_3_hier_fec_uc_rpf(0,200,201,202,30,35,100,150);
 * dnx_example_ipv4_3_hier_fec_mc_rpf(0,200,201,202,203,50);
 * exit;
 *
 * ETH-RIF packet to pass Strict uRPF check
 * tx 1 psrc = 200 data = 0x000c00020000000007000100080045000035000000008000fa44c08001027fffff02000102030405
 * Received packet on unit 0 should be:
 * Data:
 * 0x00000000cd1d00123456789a81000064080045000035000000007f00fb44c08001027fffff02000102030405
 *
 *
 * ETH-RIF packet to pass Explcit MC RPF check
 * tx 1 psrc = 200 data = 0x01005e02020a000007000100810000cc0800450000350000000080009733c0800109e002020a000102030405
 * Received packets on unit 0 should be:
 * Data:
 * 0x01005e02020a00123456789b81000065080045000035000000007f009833c0800109e002020a000102030405
 * 0x01005e02020a00123456789a81000064080045000035000000007f009833c0800109e002020a000102030405
 */

struct cint_ip_route_hierarchical_fec_info_s
{
    int in_port;                                        /** incoming port */
    int out_port;                                       /** outgoing port */
    int intf_in;                                        /** Incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address;                      /** mac for in RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                         /** mac for out RIF */
    int encap_id1;                                      /** encap ID 1 */
    int encap_id2;                                      /** encap ID 2 */
    bcm_ip_t host;                                      /** host */
    bcm_ip_t host_sip_uc;                               /** UC SIP host */
    bcm_ip_t host_mc;                                   /** MC DIP host */
    bcm_ip_t route;                                     /** route IP */
    bcm_ip_t mask;                                      /** mask of route IP */
    int vrf;                                            /** VRF */
    bcm_if_t fec_id1;                                   /** FEC id 1*/
    bcm_if_t fec_id2;                                   /** FEC id 2*/
    bcm_if_t fec_id3;                                   /** FEC id 3*/
    int new_outer_vlan;                                 /** Outer VLAN ID */
    int new_inner_vlan;                                 /** Inner VLAN ID */
    int outer_tpid;                                     /** Outer TPID */
    int inner_tpid;                                     /** Inner TPID */
};

cint_ip_route_hierarchical_fec_info_s cint_hier_fec_info =
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
     * encap_id1
     */
    0x1384,
    /*
     * encap_id2
     */
    0x1386,
    /*
     * host
     */
    0x7fffff02 /** 127.255.255.2 */,
    /*
     * host_sip_uc
     */
    0xc0800101 /** 192.128.1.1 */,
    /*
     * host_mc
     */
    0xe0020202 /** 224.2.2.2 */,
    /*
     * route
     */
    0x7fffff00 /** 127.255.255.0 */,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * vrf
     */
    1,
    /*
     * fec_id1
     */
    0x1E001,
    /*
     * fec_id2
     */
    0x10001,
    /*
     * fec_id3
     */
    0xA001,
    /*
     * new_outer_vlan
     */
    55,
    /*
     * new_inner_vlan
     */
    56,
    /*
     * outer_tpid
     */
    0x9100,
    /*
     * inner_tpid
     */
    0x8100,
};

int
out_vlan_port_intf_set(
    int unit,
    int out_port,
    int out_lif,
    bcm_gport_t * vlan_port_id)
{
    int rv = BCM_E_NONE;
    int encoded_lif;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = out_port;
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    BCM_GPORT_SUB_TYPE_LIF_SET(encoded_lif, 0, out_lif);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, encoded_lif);
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    *vlan_port_id = vlan_port.vlan_port_id;
    return rv;
}

/******* Run example ******/

/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int
ipv4_hierarchal_fec_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    bcm_gport_t lif_as_gport = 0;
    bcm_gport_t gport = 0;
    l3_egress_intf fec_1st, fec_2nd, fec_3rd;
    l3_ingress_intf ingr_itf;

    cint_hier_fec_info.in_port = in_port;
    cint_hier_fec_info.out_port = out_port;

    l3_ingress_intf_init(&ingr_itf);

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_hier_fec_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_hier_fec_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &cint_hier_fec_info.fec_id3);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_hier_fec_info.in_port, cint_hier_fec_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_hier_fec_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_hier_fec_info.intf_in, cint_hier_fec_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_hier_fec_info.intf_out, cint_hier_fec_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_hier_fec_info.intf_in;
    ingr_itf.vrf = cint_hier_fec_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }
    /*
     * 5. Create ARP for second FEC and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_hier_fec_info.encap_id1, cint_hier_fec_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 6. Create AC-Ptr for 3rd FEC and set its properties
     */
    rv = out_vlan_port_intf_set(unit, cint_hier_fec_info.out_port, cint_hier_fec_info.encap_id2, &lif_as_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_vlan_port_intf_set out_port\n");
        return rv;
    }

    /*
     * 7. Set FECs and their fields.
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_hier_fec_info.fec_id2);
    l3_egress_intf_init(&fec_1st, cint_hier_fec_info.fec_id1, cint_hier_fec_info.intf_out, 0, 0, gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_hier_fec_info.fec_id3);
    l3_egress_intf_init(&fec_2nd, cint_hier_fec_info.fec_id2, 0, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, gport);
    BCM_GPORT_LOCAL_SET(gport, cint_hier_fec_info.out_port);
    l3_egress_intf_init(&fec_3rd, cint_hier_fec_info.fec_id3, cint_hier_fec_info.intf_in, cint_hier_fec_info.encap_id2, BCM_L3_3RD_HIERARCHY, gport);

    /*
     * 8. Add VLAN editing for encap stage (using UNTAG tag_struct - 0)
     */
    rv = vlan_translate_ive_eve_translation_set(unit, lif_as_gport, cint_hier_fec_info.outer_tpid,
                                                cint_hier_fec_info.inner_tpid, bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_hier_fec_info.new_outer_vlan, cint_hier_fec_info.new_inner_vlan, 1, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    /*
     * 9. Create FECs in first, second and third hierarchy and set their properties
     */
    rv = l3__egress_only_fec__create(unit, fec_1st.fec_id, fec_1st.intf_id, fec_1st.encap_id, fec_1st.gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_2nd.fec_id, fec_2nd.intf_id, fec_2nd.encap_id, fec_2nd.gport, BCM_L3_2ND_HIERARCHY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_3rd.fec_id, fec_3rd.intf_id, fec_3rd.encap_id, fec_3rd.gport, BCM_L3_3RD_HIERARCHY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec\n");
        return rv;
    }

    /*
     * 9. Add Route entry
     */
    rv = add_route_ipv4(unit, cint_hier_fec_info.route, cint_hier_fec_info.mask, cint_hier_fec_info.vrf, fec_1st.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    /*
     * 10. Add host entry
     */
    rv = add_host_ipv4(unit, cint_hier_fec_info.host, cint_hier_fec_info.vrf, fec_1st.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    return rv;
}


/*
 * packet will be routed from in_port_loose/in_port_strict to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port_loose/in_port_strict/in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff02
 *  - SIP = 0xc0800101 / 0xc0800102 / 0xc0800103
 * expected on successful RPF lookup:
 *  - out port = out_port
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 * expected on unsuccessful RPF lookup:
 *  - packet trapped with code for failing uRPF lookup (strict or loose)
 *
 * This function configures basic configuration for strict and loose RPF for both 1st and 2nd hierachy FECs
 * when Packet is a IPv4oETH.
 *
 * in_port_loose : Incoming port on which Loose mode checks are done
 * in_port_strict : Incoming port on which Strict mode checks are done
 * vsi_loose : Incoming interface which configured as Loose mode.
 * vsi_strict : Incoming interface which configured as Strict mode.
 * vrf_loose, vrf_strict : VRF associated with vsi_loose / vsi_strict interfaces respectively.
 */
int
dnx_example_ipv4_hierarchical_fec_rpf(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int in_port,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict)
{
    int rv;
    int intf_in_loose = vsi_loose;         /* Incoming packet ETH-RIF */
    int intf_in_strict = vsi_strict;        /* Incoming packet ETH-RIF */
    int intf_in_strict_fail = vsi_strict+25;        /* Incoming packet ETH-RIF */
    int intf_in_loose_fail = vsi_loose+25;        /* Incoming packet ETH-RIF */
    sand_utils_l3_fec_s fec_structure;

    int nof_hosts = 10;
    int idx;
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_hier_fec_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_hier_fec_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    int fec = cint_hier_fec_info.fec_id1 + 16;
    int host_fecs[10] = {0, 2, 4, 6, 1,
                         8, 10, 12, 14, 9};
    int vrfs[10] = {vrf_loose, vrf_loose, vrf_loose, vrf_loose, vrf_loose,
                    vrf_strict, vrf_strict, vrf_strict, vrf_strict, vrf_strict};
    bcm_gport_t gport = 0;

    bcm_gport_t lif_as_gport = 0;
    bcm_gport_t out_gport = 0;

    cint_hier_fec_info.in_port = in_port;
    cint_hier_fec_info.out_port = out_port;

    BCM_GPORT_LOCAL_SET(out_gport, cint_hier_fec_info.out_port);
    /** interfaces related variables */
    int nof_intf = 5;
    l3_port_intf intfs[5];
    l3_port_intf_init(&intfs[0], in_port_loose, vrf_loose, intf_in_loose, bcmL3IngressUrpfLoose, cint_hier_fec_info.intf_in_mac_address);
    l3_port_intf_init(&intfs[1], in_port_strict, vrf_strict, intf_in_strict, bcmL3IngressUrpfStrict, cint_hier_fec_info.intf_in_mac_address);
    l3_port_intf_init(&intfs[2], cint_hier_fec_info.in_port, vrf_strict, intf_in_strict_fail, bcmL3IngressUrpfStrict, cint_hier_fec_info.intf_in_mac_address);
    l3_port_intf_init(&intfs[3], cint_hier_fec_info.in_port, vrf_loose, intf_in_loose_fail, bcmL3IngressUrpfLoose, cint_hier_fec_info.intf_in_mac_address);
    l3_port_intf_init(&intfs[4], 0, 0, cint_hier_fec_info.intf_out, 0, cint_hier_fec_info.intf_out_mac_address);

    /** FEC-related variables */
    int nof_fecs = 16;
    int fec_ids[16] = {
        /** regarding loose RPF */
        cint_hier_fec_info.fec_id1, cint_hier_fec_info.fec_id2, cint_hier_fec_info.fec_id1 + 2,
        cint_hier_fec_info.fec_id2 + 2, cint_hier_fec_info.fec_id1 + 4, cint_hier_fec_info.fec_id2 + 4,
        cint_hier_fec_info.fec_id1 + 6, cint_hier_fec_info.fec_id2 + 6,
        /** regarding strict RPF */
        cint_hier_fec_info.fec_id1 + 8, cint_hier_fec_info.fec_id2 + 8, cint_hier_fec_info.fec_id1 + 10,
        cint_hier_fec_info.fec_id2 + 10, cint_hier_fec_info.fec_id1 + 12, cint_hier_fec_info.fec_id2 + 12,
        cint_hier_fec_info.fec_id1 + 14, cint_hier_fec_info.fec_id2 + 14
    };

    l3_egress_intf fecs[16];

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /** 1. Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }

    /*
     * 2. Set In-Port to In ETh-RIF
     *   - loose
     *   - strict
     */
    for (idx = 0; idx < nof_intf; idx++)
    {
        if (intfs[idx].port != 0)
        {
            rv = cint_ip_rpf_port_intf_set(unit, intfs[idx].port, intfs[idx].intf_id, intfs[idx].vrf, intfs[idx].rpf_mode);
            if (rv != BCM_E_NONE)
            {
                printf("Error, cint_ip_rpf_port_intf_set\n");
                return rv;
            }
        }
        else
        {
            /*
             * Set Out-Port default properties
             */
            rv = out_port_set(unit, cint_hier_fec_info.out_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, out_port_set\n");
                return rv;
            }
            rv = intf_eth_rif_create(unit, intfs[idx].intf_id, intfs[idx].mac_addr);
            if (rv != BCM_E_NONE)
            {
                printf("Error, intf_eth_rif_create iter = %d, intf = %d\n", idx, intfs[idx].intf_id);
                return rv;
            }
        }
    }

    rv = l3__egress_only_encap__create(unit, 0, &cint_hier_fec_info.encap_id1, cint_hier_fec_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }
    /** loose */
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[1]);
    l3_egress_intf_init(fecs[0], fec_ids[0], intf_in_loose, 0, 0, gport);
    l3_egress_intf_init(fecs[1], fec_ids[1], intf_in_loose, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[3]);
    l3_egress_intf_init(fecs[2], fec_ids[2], intf_in_loose, 0, 0, gport);
    l3_egress_intf_init(fecs[3], fec_ids[3], intf_in_loose_fail, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[5]);
    l3_egress_intf_init(fecs[4], fec_ids[4], intf_in_loose_fail, 0, 0, gport);
    l3_egress_intf_init(fecs[5], fec_ids[5], intf_in_loose, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[7]);
    l3_egress_intf_init(fecs[6], fec_ids[6], intf_in_loose_fail, 0, 0, gport);
    l3_egress_intf_init(fecs[7], fec_ids[7], intf_in_loose_fail, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    /** strict */
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[9]);
    l3_egress_intf_init(fecs[8], fec_ids[8], intf_in_strict, 0, 0, gport);
    l3_egress_intf_init(fecs[9], fec_ids[9], intf_in_strict, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[11]);
    l3_egress_intf_init(fecs[10], fec_ids[10], intf_in_strict, 0, 0, gport);
    l3_egress_intf_init(fecs[11], fec_ids[11], cint_hier_fec_info.intf_out, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[13]);
    l3_egress_intf_init(fecs[12], fec_ids[12], intf_in_strict_fail, 0, 0, gport);
    l3_egress_intf_init(fecs[13], fec_ids[13], intf_in_strict, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_ids[15]);
    l3_egress_intf_init(fecs[14], fec_ids[14], intf_in_strict_fail, 0, 0, gport);
    l3_egress_intf_init(fecs[15], fec_ids[15], intf_in_strict_fail, cint_hier_fec_info.encap_id1, BCM_L3_2ND_HIERARCHY, out_gport);

    /*
     * 7. Create FECs and set their properties
     */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination = fecs[idx].gport;
        fec_structure.tunnel_gport = fecs[idx].intf_id;
        fec_structure.tunnel_gport2 = fecs[idx].encap_id;
        fec_structure.fec_id = fecs[idx].fec_id;
        fec_structure.flags = fecs[idx].flags;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
    }
    /*
     * 8. Create FEC for DIP host entries
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, cint_hier_fec_info.out_port);
    fec_structure.tunnel_gport = cint_hier_fec_info.intf_out;
    fec_structure.tunnel_gport2 = cint_hier_fec_info.encap_id1;
    fec_structure.fec_id = fec;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 9. Create AC-Ptr for 2nd FEC and set its properties
     */
    rv = out_vlan_port_intf_set(unit, cint_hier_fec_info.out_port, cint_hier_fec_info.encap_id2, &lif_as_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_vlan_port_intf_set intf_in\n");
        return rv;
    }
    rv = vlan_translate_ive_eve_translation_set(unit, lif_as_gport, cint_hier_fec_info.outer_tpid, cint_hier_fec_info.inner_tpid, bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_hier_fec_info.new_outer_vlan, cint_hier_fec_info.new_inner_vlan, 1, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    /**
     *  10. Add host entries for DIPs and SIPs
     */
    /*
     * Loose mode entries - two for successful RPF check
     */
    rv = add_host_ipv4(unit, cint_hier_fec_info.host, vrf_loose, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, dip, vrf_loose\n");
        return rv;
    }
    rv = add_host_ipv4(unit, cint_hier_fec_info.host + 1, vrf_strict, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, dip, vrf_loose\n");
        return rv;
    }

    /*
     * host entries
     */
    for (idx = 0; idx < nof_hosts; idx++)
    {
        printf("%d - Creating host entry for SIP = %04x, vrf = %d and FEC = %04x\n", idx,
               cint_hier_fec_info.host_sip_uc + idx, vrfs[idx], fec_ids[host_fecs[idx]]);
        rv = add_host_ipv4(unit, cint_hier_fec_info.host_sip_uc + idx, vrfs[idx], fec_ids[host_fecs[idx]], 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_host_ipv4, sip_1, vrf_loose, fec\n");
            return rv;
        }
    }

    return rv;
}

/*
 * packet will be routed from in_port_loose/in_port_strict to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port_loose/in_port_strict
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff02
 *  - SIP = 0xc0800101 - 0xc0800104
 * expected on successful RPF lookup:
 *  - out port = out_port
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 * expected on unsuccessful RPF lookup:
 *  - packet trapped with code for failing uRPF lookup (strict or loose)
 *
 * This function configures basic configuration for strict and loose RPF for 1st, 2nd and 3rd hierachy FECs
 * when Packet is a IPv4oETH.
 *
 * in_port_loose : Incoming port on which Loose mode checks are done
 * in_port_strict : Incoming port on which Strict mode checks are done
 * out_port : Out port for the out packet in case of successful RPF lookup.
 * vsi_loose : Incoming interface which configured as Loose mode.
 * vsi_strict : Incoming interface which configured as Strict mode.
 * vrf_loose, vrf_strict : VRF associated with vsi_loose / vsi_strict interfaces respectively.
 *
 *
 * 1. FEC_1H (incorrect RIF) +-------> FEC_2H (incorrect RIF) +-------> FEC_3H (incorrect RIF)-->+--------+
 * 2. FEC_1H (correct RIF) +---------> FEC_2H (incorrect RIF) +-------> FEC_3H (incorrect RIF)-->|out port|
 * 3. FEC_1H (incorrect RIF) +-------> FEC_2H (correct RIF) +---------> FEC_3H (incorrect RIF)-->+--------+
 * 4. FEC_1H (incorrect RIF) +-------> FEC_2H (incorrect RIF) +-------> FEC_3H (correct RIF)---->
 *
 * Cases 1 to 4 should pass with Loose RPF check.
 * Case 2 should pass, cases 1, 3, 4 should fail in strict RPF mode.
 */
int
dnx_example_ipv4_3_hier_fec_uc_rpf(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict)
{
    int rv;
    int intf_in_loose = vsi_loose;         /* Incoming packet ETH-RIF */
    int intf_in_strict = vsi_strict;        /* Incoming packet ETH-RIF */

    bcm_gport_t gport = 0;
    int vrfs[2] =  {vrf_loose, vrf_strict};
    int mid;
    int nof_rpf_modes = 2;
    l3_port_intf intfs[2];
    l3_port_intf_init(&intfs[0], in_port_loose, vrf_loose, intf_in_loose, bcmL3IngressUrpfLoose, cint_hier_fec_info.intf_in_mac_address);
    l3_port_intf_init(&intfs[1], in_port_strict, vrf_strict, intf_in_strict, bcmL3IngressUrpfStrict, cint_hier_fec_info.intf_in_mac_address);

    int nof_sips = 4;
    int nof_fecs = 4;
    int nof_fecs_loc;
    int host_sip_loc;
    int idx;

    cint_hier_fec_info.out_port = out_port;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_hier_fec_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_hier_fec_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &cint_hier_fec_info.fec_id3);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    l3_egress_intf fecs_1st[8];
    l3_egress_intf fecs_2nd[8];
    l3_egress_intf fecs_3rd[8];
    /** loose */
    l3_egress_intf_init(fecs_1st[0], cint_hier_fec_info.fec_id1 + 2, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[1], cint_hier_fec_info.fec_id1 + 4, intfs[0].intf_id, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[2], cint_hier_fec_info.fec_id1 + 6, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[3], cint_hier_fec_info.fec_id1 + 8, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[4], cint_hier_fec_info.fec_id1 + 10, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[5], cint_hier_fec_info.fec_id1 + 12, intfs[1].intf_id, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[6], cint_hier_fec_info.fec_id1 + 14, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_1st[7], cint_hier_fec_info.fec_id1 + 16, cint_hier_fec_info.intf_out, 0, 0, 0);

    l3_egress_intf_init(fecs_2nd[0], cint_hier_fec_info.fec_id2, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[1], cint_hier_fec_info.fec_id2 + 2, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[2], cint_hier_fec_info.fec_id2 + 4, intfs[0].intf_id, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[3], cint_hier_fec_info.fec_id2 + 6, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[4], cint_hier_fec_info.fec_id2 + 8, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[5], cint_hier_fec_info.fec_id2 + 10, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[6], cint_hier_fec_info.fec_id2 + 12, intfs[1].intf_id, 0, 0, 0);
    l3_egress_intf_init(fecs_2nd[7], cint_hier_fec_info.fec_id2 + 14, cint_hier_fec_info.intf_out, 0, 0, 0);

    l3_egress_intf_init(fecs_3rd[0], cint_hier_fec_info.fec_id3, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[1], cint_hier_fec_info.fec_id3 + 1, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[2], cint_hier_fec_info.fec_id3 + 2, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[3], cint_hier_fec_info.fec_id3 + 3, intfs[0].intf_id, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[4], cint_hier_fec_info.fec_id3 + 4, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[5], cint_hier_fec_info.fec_id3 + 5, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[6], cint_hier_fec_info.fec_id3 + 6, cint_hier_fec_info.intf_out, 0, 0, 0);
    l3_egress_intf_init(fecs_3rd[7], cint_hier_fec_info.fec_id3 + 7, intfs[1].intf_id, 0, 0, 0);

    /** 1. Configure traps for UC RPF - Loose and Strict */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }

    /** 2. Set Out-Port default properties */
    rv = out_port_set(unit, cint_hier_fec_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    /** 3. Configure in-port, ingress interfaces and set RPF check */
    for (mid = 0; mid < nof_rpf_modes; mid++)
    {
        printf("Creating intf = %d with port = %d\n", intfs[mid].port, intfs[mid].intf_id);
        rv = cint_ip_rpf_port_intf_set(unit, intfs[mid].port, intfs[mid].intf_id, intfs[mid].vrf, intfs[mid].rpf_mode);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_ip_rpf_port_intf_set\n");
            return rv;
        }
    }

    BCM_GPORT_LOCAL_SET(gport, cint_hier_fec_info.out_port);
    /** 4. Create OUT-LIF and set its properties: FEC, encap-id */
    rv = intf_eth_rif_create(unit, cint_hier_fec_info.intf_out, cint_hier_fec_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    rv = l3__egress_only_encap__create(unit, 0, &cint_hier_fec_info.encap_id1, cint_hier_fec_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }
    /** 5. Create FEC in 1H pointing to out port. Will be used to DIP host entry destination */
    rv = l3__egress_only_fec__create(unit, cint_hier_fec_info.fec_id1, cint_hier_fec_info.intf_out, cint_hier_fec_info.encap_id1, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_out\n");
        return rv;
    }

    /** 6. Iterate over the two RPF modes and create the needed entries.*/
    for (mid = 0; mid < nof_rpf_modes; mid++)
    {
        host_sip_loc = cint_hier_fec_info.host_sip_uc;
        nof_fecs_loc = (mid + 1) * nof_fecs;
        /*
         * 6.1. Create the FEC entries - 1H_FEC -> 2H_FEC -> 3H_FEC
         */
        for (idx = mid * nof_fecs; idx < nof_fecs_loc; idx++)
        {
            BCM_GPORT_FORWARD_PORT_SET(gport, fecs_2nd[idx].fec_id);
            rv = l3__egress_only_fec__create(unit, fecs_1st[idx].fec_id, fecs_1st[idx].intf_id, 0, gport, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only, fec\n");
                return rv;
            }
            BCM_GPORT_FORWARD_PORT_SET(gport, fecs_3rd[idx].fec_id);
            rv = l3__egress_only_fec__create(unit, fecs_2nd[idx].fec_id, fecs_2nd[idx].intf_id, 0, gport, BCM_L3_2ND_HIERARCHY);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only, fec\n");
                return rv;
            }
            BCM_GPORT_LOCAL_SET(gport, out_port);
            rv = l3__egress_only_fec__create(unit, fecs_3rd[idx].fec_id, fecs_3rd[idx].intf_id, 0, gport, BCM_L3_3RD_HIERARCHY);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only, fec\n");
                return rv;
            }
        }

        /*
         * 6.2. Add host entries
         * SIP host entries using eash 1H FEC.
         * DIP host entry.
         */
        for (idx = mid * nof_sips; idx < nof_sips * (mid + 1); idx++)
        {
            rv = add_host_ipv4(unit, host_sip_loc, vrfs[mid], fecs_1st[idx].fec_id, 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in function add_host, sip, vrfs[mid], fec\n");
                return rv;
            }
            host_sip_loc++;
        }
        rv = add_host_ipv4(unit, cint_hier_fec_info.host, vrfs[mid], cint_hier_fec_info.fec_id1, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_host, dip, vrfs[mid]\n");
            return rv;
        }
    }

    return rv;
}

/*
 * packet will be routed from in_port to out-port1 and out_port2
 *
 * Route:
 * It is expected that the input packet will have
 *      * DA = 01:00:5E:02:02:02
 *      * SA = 00:00:07:00:01:00
 *      * VID = rif (input)
 *      * DIP = 224.2.2.2 - 224.2.2.9
 *      * SIP = 192.128.1.1
 *
 * example:
 * int unit = 0;
 * int in_port = 200;
 * int out_port1 = 201;
 * int out_port2 = 202;
 * int out_port3 = 203;
 * int rif = 0x50;
 */
/*
 * This function sets basic configuration for SIP-based and explicit RPF for 1st, 2nd and 3rd hierachy FECs
 * when Packet is a IPv4oETH.
 * SIP-based RPF check requires existing <VRF,SIP> entry matching packet SIP.
 * Explicit RPF check requires matching packet RIF.
 *
 * in_port : Incoming port
 * out_port1, out_port2 : Outgoing ports used for multicast replications
 * rif : ETH RIF
 */
int
dnx_example_ipv4_3_hier_fec_mc_rpf(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int rif)
{
    int rv;
    int wrong_rif = rif + 5;
    int intf_out = cint_hier_fec_info.intf_out;

    bcm_mac_t out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };   /* next_hop_mac */

    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0;

    int ipmc_group = 6000;
    int idx = 0;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int nof_fecs = 10;
    bcm_multicast_replication_t replications[2];
    int out_ports[2] = {out_port1, out_port2};
    bcm_gport_t gport;

    cint_hier_fec_info.in_port = in_port;
    cint_hier_fec_info.intf_in = rif;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_hier_fec_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_hier_fec_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &cint_hier_fec_info.fec_id3);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    l3_egress_intf fec_1st[10];
    l3_egress_intf fec_2nd[10];
    l3_egress_intf fec_3rd[10];
    /*
     * 6*3 FECs regarding SIP-based RPF check will be created;
     * 6*3 FECs regarding Explicit RPF check will be created.
     */
    l3_egress_intf_init(fec_1st[0], cint_hier_fec_info.fec_id1, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_1st[1], cint_hier_fec_info.fec_id1 + 2, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_1st[2], cint_hier_fec_info.fec_id1 + 4, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_1st[3], cint_hier_fec_info.fec_id1 + 6, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_1st[4], cint_hier_fec_info.fec_id1 + 8, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_1st[5], cint_hier_fec_info.fec_id1 + 10, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_1st[6], cint_hier_fec_info.fec_id1 + 12, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_1st[7], cint_hier_fec_info.fec_id1 + 14, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_1st[8], cint_hier_fec_info.fec_id1 + 16, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_1st[9], cint_hier_fec_info.fec_id1 + 18, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);

    l3_egress_intf_init(fec_2nd[0], cint_hier_fec_info.fec_id2, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_2nd[1], cint_hier_fec_info.fec_id2 + 2, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_2nd[2], cint_hier_fec_info.fec_id2 + 4, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_2nd[3], cint_hier_fec_info.fec_id2 + 6, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_2nd[4], cint_hier_fec_info.fec_id2 + 8, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_2nd[5], cint_hier_fec_info.fec_id2 + 10, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_2nd[6], cint_hier_fec_info.fec_id2 + 12, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_2nd[7], cint_hier_fec_info.fec_id2 + 14, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_2nd[8], cint_hier_fec_info.fec_id2 + 16, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_2nd[9], cint_hier_fec_info.fec_id2 + 18, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_EXPLICIT, 0);

    l3_egress_intf_init(fec_3rd[0], cint_hier_fec_info.fec_id3, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_3rd[1], cint_hier_fec_info.fec_id3 + 1, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_3rd[2], cint_hier_fec_info.fec_id3 + 2, wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_3rd[3], cint_hier_fec_info.fec_id3 + 3, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_3rd[4], cint_hier_fec_info.fec_id3 + 4, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_SIP_BASE, 0);
    l3_egress_intf_init(fec_3rd[5], cint_hier_fec_info.fec_id3 + 5, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_3rd[6], cint_hier_fec_info.fec_id3 + 6, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_3rd[7], cint_hier_fec_info.fec_id3 + 7, wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_3rd[8], cint_hier_fec_info.fec_id3 + 8, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_EXPLICIT, 0);
    l3_egress_intf_init(fec_3rd[9], cint_hier_fec_info.fec_id3 + 9, cint_hier_fec_info.intf_in, 0, BCM_L3_MC_RPF_EXPLICIT, 0);

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /** 1. Config traps  */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

    /*
     * 2. Set In-Port and Out-Port to In ETh-RIF
     *   - loose
     *   - strict
     */
    rv = cint_ip_rpf_port_intf_set(unit, cint_hier_fec_info.in_port, cint_hier_fec_info.intf_in, cint_hier_fec_info.vrf, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set\n");
        return rv;
    }

    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port1\n");
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port2\n");
        return rv;
    }
    rv = out_port_set(unit, out_port3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port3\n");
        return rv;
    }

    /*
     * 3. Set ingress interface
     */
    ingr_itf.intf_id = cint_hier_fec_info.intf_in;
    ingr_itf.vrf = cint_hier_fec_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, ipmc_group, out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }


    /*
     * 5. Define expected out DMAC
     */
    rv = l3__egress_only_encap__create(unit, 0, &cint_hier_fec_info.encap_id1, next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }

    /*
     * 6. Create FECs for SIP-based and Explicit RPF
     * The seven FECs for each RPF mode that will be created are:
     *      * RPF FEC_1st_hier(RIF) -> RPF FEC_2nd_hier(wrong RIF) -> MC group
     *      * RPF FEC_1st_hier (wrong RIF) -> RPF FEC_2nd_hier (RIF) -> MC group
     *      * RPF FEC_1st_hier (wrong RIF) -> RPF FEC_2nd_hier (wrong RIF) -> MC group
     *      * RPF FEC_2nd_hier (RIF) -> MC group
     * For each of these combinations an IPMC entry will be created.
     */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        /** Create FEC in 1st hierarchy pointing to a FEC in 2nd hierarchy */
        BCM_GPORT_FORWARD_PORT_SET(gport, fec_2nd[idx].fec_id);
        printf("%d - Creating FEC 1st hier with ID = %04x, rif = %d, encap = %d, gport = %04x\n", idx,
               fec_1st[idx].fec_id, fec_1st[idx].intf_id, 0, gport);
        rv = l3__egress_only_fec__create(unit, fec_1st[idx].fec_id, fec_1st[idx].intf_id, 0, gport, fec_1st[idx].flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fec_1st[%d]\n", idx);
            return rv;
        }
        /** Create an IPMC entry that results to a FEC_1st_hier */
        printf("%d - Creating IPMC entry with DIP = %04x, rif = %u, fec = %04x\n", idx, cint_hier_fec_info.host_mc,
               cint_hier_fec_info.intf_in, fec_1st[idx].fec_id);
        rv = add_ipv4_ipmc(unit, cint_hier_fec_info.host_mc, dip_mask, 0, sip_mask, cint_hier_fec_info.intf_in,
                           cint_hier_fec_info.vrf, 0, fec_1st[idx].fec_id, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv4_ipmc - iter = %d \n", idx);
            return rv;
        }
        cint_hier_fec_info.host_mc++;

        /** Create FEC in 2nd hierarchy that points to a FEC in 3rd */
        BCM_GPORT_FORWARD_PORT_SET(gport, fec_3rd[idx].fec_id);
        printf("%d - Creating FEC 2nd hier with ID = %04x, rif = %d, encap = %d, gport = %04x\n", idx,
               fec_2nd[idx].fec_id, fec_2nd[idx].intf_id, 0, gport);
        rv = l3__egress_only_fec__create(unit, fec_2nd[idx].fec_id, fec_2nd[idx].intf_id, 0, gport,
                                         fec_2nd[idx].flags | BCM_L3_2ND_HIERARCHY);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fec_2nd[%d]\n", idx);
            return rv;
        }

        /** Create a FEC in 3rd hierarchy pointing to the MC group */
        BCM_GPORT_MCAST_SET(gport, ipmc_group);
        printf("%d - Creating FEC 3rd hier with ID = %04x, rif = %d, encap = %d, gport = %04x\n", idx,
               fec_3rd[idx].fec_id, fec_3rd[idx].intf_id, 0, gport);
        rv = l3__egress_only_fec__create(unit, fec_3rd[idx].fec_id, fec_3rd[idx].intf_id, 0, gport,
                                         fec_3rd[idx].flags | BCM_L3_3RD_HIERARCHY);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs_3rd[%d]\n", idx);
            return rv;
        }
        rv = add_host_ipv4(unit, cint_hier_fec_info.host_sip_uc, cint_hier_fec_info.vrf, fec_1st[idx].fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4 - host_sip, vrf, fecs[0]\n");
            return rv;
        }
        cint_hier_fec_info.host_sip_uc++;
    }

    return rv;
}

/*
 * packet will be routed from in_port to out-port1 and out_port2
 *
 * Route:
 * It is expected that the input packet will have
 *      * DA = 01:00:5E:02:02:02
 *      * SA = 00:00:07:00:01:00
 *      * VID = rif (input)
 *      * DIP = 224.2.2.2 - 224.2.2.9
 *      * SIP = 192.128.1.1
 *
 * example:
 * int unit = 0;
 * int in_port = 200;
 * int out_port1 = 201;
 * int out_port2 = 202;
 * int out_port3 = 203;
 * int rif = 0x50;
 */
/*
 * This function sets basic configuration for SIP-based and explicit RPF for both 1st and 2nd hierachy FECs
 * when Packet is a IPv4oETH.
 * SIP-based RPF check requires existing <VRF,SIP> entry matching packet SIP.
 * Explicit RPF check requires matching packet RIF.
 *
 * in_port : Incoming port
 * out_port1, out_port2 : Outgoing ports used for multicast replications
 * rif : ETH RIF
 */
int
dnx_example_ipv4_hierarchical_fec_mc_rpf(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int rif)
{
    int rv;
    int intf_in = rif;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */

    int encap_id = 6520;
    int encap_id_2 = 902;       /* Out-Ac-Ptr */
    int vrf = 100;
    int wrong_rif = rif + 5;

    bcm_mac_t my_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_mac_t out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };    /* next_hop_mac */

    uint32 host_dip = 0xe0020202;
    uint32 host_sip = 0xc0800101;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0;

    int new_outer_vlan = 55;
    int new_inner_vlan = 56;
    int ipmc_group = 6000;
    int idx = 0;
    int hid = 0;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int nof_fecs = 14;
    int nof_ipmc_entries = 8;
    /** FEC Ids that will be used for the creation of the IPMC entries */
    int fec_ids[8] = { 0, 2, 4, 6,/** sip-based */
        7, 9, 11, 13
    };                               /** explicit */
    bcm_gport_t mc_gport;
    bcm_gport_t gport;
    BCM_GPORT_MCAST_SET(mc_gport, ipmc_group);
    /** FEC Ids that will be used for the creation of the IPMC entries */
    int host_fec_ids[8] = { 0, 2, 4, 6,/** sip-based */
        7, 9, 11, 13
    };                               /** explicit */
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_hier_fec_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_hier_fec_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    /** IDs of the FECs that will be created */
    int fec_idxs[14] = { cint_hier_fec_info.fec_id1, cint_hier_fec_info.fec_id2, cint_hier_fec_info.fec_id1 + 2,
        cint_hier_fec_info.fec_id2 + 2, cint_hier_fec_info.fec_id1 + 4, cint_hier_fec_info.fec_id2 + 4, cint_hier_fec_info.fec_id2 + 6, /** sip-based */
        cint_hier_fec_info.fec_id1 + 6, cint_hier_fec_info.fec_id2 + 8, cint_hier_fec_info.fec_id1 + 8, cint_hier_fec_info.fec_id2 + 10,
        cint_hier_fec_info.fec_id1 + 10, cint_hier_fec_info.fec_id2 + 12, cint_hier_fec_info.fec_id2 + 14 /** explicit */
    };
    l3_egress_intf fecs[14];
    /** sip-based RPF*/
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[1]);
    l3_egress_intf_init(fecs[0], fec_idxs[0], rif, 0, BCM_L3_MC_RPF_SIP_BASE, gport);
    l3_egress_intf_init(fecs[1], fec_idxs[1], wrong_rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_SIP_BASE, mc_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[3]);
    l3_egress_intf_init(fecs[2], fec_idxs[2], wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, gport);
    l3_egress_intf_init(fecs[3], fec_idxs[3], rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_SIP_BASE, mc_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[5]);
    l3_egress_intf_init(fecs[4], fec_idxs[4], wrong_rif, 0, BCM_L3_MC_RPF_SIP_BASE, gport);
    l3_egress_intf_init(fecs[5], fec_idxs[5], wrong_rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_SIP_BASE, mc_gport);
    l3_egress_intf_init(fecs[6], fec_idxs[6], rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_SIP_BASE, mc_gport);
    /** explicit RPF*/
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[8]);
    l3_egress_intf_init(fecs[7], fec_idxs[7], rif, 0, BCM_L3_MC_RPF_EXPLICIT, gport);
    l3_egress_intf_init(fecs[8], fec_idxs[8], wrong_rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_EXPLICIT, mc_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[10]);
    l3_egress_intf_init(fecs[9], fec_idxs[9], wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, gport);
    l3_egress_intf_init(fecs[10], fec_idxs[10], rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_EXPLICIT, mc_gport);
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_idxs[12]);
    l3_egress_intf_init(fecs[11], fec_idxs[11], wrong_rif, 0, BCM_L3_MC_RPF_EXPLICIT, gport);
    l3_egress_intf_init(fecs[12], fec_idxs[12], wrong_rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_EXPLICIT, mc_gport);
    l3_egress_intf_init(fecs[13], fec_idxs[13], rif, 0, BCM_L3_2ND_HIERARCHY | BCM_L3_MC_RPF_EXPLICIT, mc_gport);

    bcm_multicast_replication_t replications[2];
    int out_ports[2] = { out_port1, out_port2 };

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    if (is_device_or_above(unit, JERICHO2))
    {
        /** out-LIFs for jr2 must be > 2k*/
        encap_id = 0x1384;
        encap_id_2 = 0x1386;
    }

    /** 1. Config traps  */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

    /*
     * 2. Set In-Port to In ETh-RIF
     *   - loose
     *   - strict
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_in, my_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port1\n");
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port2\n");
        return rv;
    }
    rv = out_port_set(unit, out_port3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port3\n");
        return rv;
    }

    /*
     * 3. Set ingress interface
     */
    ingr_itf.intf_id = intf_in;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, ipmc_group, out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }

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
     * 6. Create FECs for SIP-based and Explicit RPF
     * The seven FECs for each RPF mode that will be created are:
     *      * RPF FEC_1st_hier(RIF) -> RPF FEC_2nd_hier(wrong RIF) -> MC group
     *      * RPF FEC_1st_hier (wrong RIF) -> RPF FEC_2nd_hier (RIF) -> MC group
     *      * RPF FEC_1st_hier (wrong RIF) -> RPF FEC_2nd_hier (wrong RIF) -> MC group
     *      * RPF FEC_2nd_hier (RIF) -> MC group
     * For each of these combinations an IPMC entry will be created.
     */
    for (idx = 0, hid = 0; idx < nof_fecs; idx++)
    {
        printf("%d - Creating FEC with ID = %d, rif = %d, encap = %d, gport = 0x%04x\n", idx, fecs[idx].fec_id,
               fecs[idx].intf_id, 0, fecs[idx].gport);
        rv = l3__egress_only_fec__create(unit, fecs[idx].fec_id, fecs[idx].intf_id, fecs[idx].encap_id, fecs[idx].gport,
                                         fecs[idx].flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs[%d]\n", idx);
            return rv;
        }
        if (hid < 8)
        {
            /*
             * 7. Create eight IPMC entries - four leading to SIP-based RPF, four leading to Explicit RPF.
             * four to 1st hier FEC, four leading to 2nd hier FEC
             */
            if (fec_ids[hid] == idx)
            {
                printf("%d - Creating IPMC entry with DIP = %u, rif = %u, fec = %d (id = %d)\n", idx, host_dip, rif,
                       fecs[idx].fec_id, idx);
                rv = add_ipv4_ipmc(unit, host_dip, dip_mask, 0, sip_mask, rif, vrf, 0, fecs[idx].fec_id, 0);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in function add_ipv4_ipmc - iter = %d \n", idx);
                    return rv;
                }
                rv = add_host_ipv4(unit, host_sip, vrf, fecs[idx].fec_id, 0, 0);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, add_host_ipv4 - host_sip, vrf, fecs\n");
                    return rv;
                }
                host_dip++;
                host_sip++;
                hid++;
            }
        }
    }

    return rv;
}
