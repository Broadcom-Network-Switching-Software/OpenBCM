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
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/dpp/cint_ip_route_basic.c
 * cint
 * basic_example(0,200,202);
 * exit;
 *
 * ETH-RIF packet 
 * tx 1 psrc = 200 data = 0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be: 
 * Data:
 * 0x00000000cd1d000c0002000181000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 */

int in_rif_id = 0;
/*
 * Utilities APIs
 */

/*
 * Set In-VSI ETH-RIF properties:
 * - eth_rif_id: ETH-RIF ID
 * - vrf: VRF ID 
 * - qos_map_id: QOS_MAP_ID 
 */
int
qos_intf_ingress_rif_set(
    int unit,
    int eth_rif_id,
    int vrf,
    int qos_map_id)
{
    bcm_l3_ingress_t l3_ing_if;
    int rc;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.qos_map_id = qos_map_id;

    rc = bcm_l3_ingress_create(unit, l3_ing_if, eth_rif_id);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }

    return rc;
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
 * host_format options:
 * host_format = 0 : FEC
 * host_format = 1 : FEC + OUTLIF
 * host_format = 2 : System-Port, OutRIF, ARP (No-FEC)
 */
/*
 * example: 
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 int in_qos_map_id = 7;
 */
int
basic_example_qos_inner(
    int unit,
    int in_port,
    int out_port,
    int host_format,
    int ing_qos_map_id,
    int egr_qos_map_id)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec = 40961;
    int vrf = 1;
    int encap_id = 900;         /* ARP-Link-layer */
    int host_encap_id = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    int in_qos_map_id;
    int out_qos_map_id;

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,
                                       ing_qos_map_id, &in_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, egr_qos_map_id, &out_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1384;
    }

    /*
     * 1. Set In-Port to In ETh-RIF 
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties 
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create_with_qos(unit, intf_out, out_qos_map_id, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties 
     */
    rv = qos_intf_ingress_rif_set(unit, intf_in, vrf, in_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 5. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (host_format != 2)
    {
        sand_utils_l3_fec_s fec_structure;
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
        fec_structure.tunnel_gport = intf_out;
        fec_structure.tunnel_gport2 = encap_id;
        fec_structure.fec_id = fec;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        fec_structure.qos_map_id = out_qos_map_id;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }
        
    }

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &encap_id, arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 7. Add Route entry
     */
    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }

    /*
     * 7. Add host entry
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    if (host_format == 1)
    {
        rv = add_host_ipv4(unit, host, vrf, fec, intf_out, 0);
    }
    else if (host_format == 2)
    {
        rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
    }
    else
    {
        rv = add_host_ipv4(unit, host, vrf, fec, 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host, \n");
        return rv;
    }

    return rv;
}

int
basic_example_qos(
    int unit,
    int in_port,
    int out_port,
    int in_qos_map_id,
    int out_qos_map_id)
{
    return basic_example_qos_inner(unit, in_port, out_port, 0, in_qos_map_id, out_qos_map_id);
}

/*
 * Test Scenario
 *   test functionality of IPv4 host with FEC-ptr.
 *   host lookup returns: <FEC-ptr >
 *   FEC lookup returns: < ourRIF, next-hop-MAC address(ARP-ID) >
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_basic_qos.c
 * cint
 * lif_cos_and_remark_example(0,200,201);
 * exit;
 */
static int ing_qos_map_id = 7;
static int egr_qos_map_id = 2;

int
lif_cos_and_remark_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int ing_intf_in;
    int ing_intf_out;
    int fec = 40961;
    int encap_id = 0;
    int flags = 0;
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 15;
    int host = 0x0a00ff01;
    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    bcm_mac_t my_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x03, 0x00, 0x01 };
    bcm_l3_host_t l3host;
    bcm_if_t encoded_fec;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    /*
     * 1. create ingress router interface
     */
    rv = bcm_vlan_create(unit, in_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, create vlan=%d, in unit %d \n", in_vlan, unit);
    }

    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, 0, my_mac_address, vrf);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,ing_qos_map_id, &eth_rif_structure.ing_qos_map_id);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,egr_qos_map_id, &eth_rif_structure.egr_qos_map_id);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_in = eth_rif_structure.l3_rif;
    in_rif_id = ing_intf_in;

    /*
     * 2. create egress router interface
     */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, create vlan=%d, in unit %d \n", out_vlan, unit);
    }

    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, my_mac_address, vrf);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,ing_qos_map_id, &eth_rif_structure.ing_qos_map_id);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,egr_qos_map_id, &eth_rif_structure.egr_qos_map_id);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_out = eth_rif_structure.l3_rif;

    /*
     * 3. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 4. build FEC points to out-RIF and next-hop MAC address
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        fec = 20 * 1024 + 1;
    }
    rv = l3__egress_only_fec__create_inner(unit, fec, ing_intf_out, encap_id, out_port, 0, 0, 0, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    if (verbose >= 1)
    {
        printf("created FEC-id =0x%08x\n", fec);
        printf("encap-id = 0x%08x\n", encap_id);
    }

    if (verbose >= 2)
    {
        printf("outRIF = 0x%08x out-port =%d\n", ing_intf_out, fec);
    }

    /*
     * 5. add host entry with FEC-ptr
     */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = encoded_fec;      /* FEC */

    /*
     * as encap id is valid (!=0), host will point to FEC + outlif
     */
    l3host.encap_id = 0;

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rv);
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }

    if (verbose >= 1)
    {
        print_host("add entry ", host, vrf);
        printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);
    }

    return rv;
}

int
ip_route_qos_pcp_dscp(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int in_vlan = 15;
    int out_vlan = 100;
    int in_vlan_port_id;
    int ing_intf_in;
    int ing_intf_out;
    int fec = 40961;
    int arp_encap_id = 0;
    int flags = 0;

    int vrf = 15;
    int host = 0x0a00ff01;
    bcm_vlan_control_vlan_t control_vlan;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    bcm_mac_t my_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x03, 0x00, 0x01 };
    bcm_l3_host_t l3host;
    bcm_if_t encoded_fec;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }



    /*
     * 1. Set in-AC to in-RIF
     */
    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 0x1010);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.vsi = in_vlan;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_WITH_ID;
    vlan_port.vlan_port_id = in_vlan_port_id;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create vlan=%d, in unit %d \n", in_vlan, unit);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add vlan=%d, error = %d \n", in_vlan, unit);
        return rv;
    }

    if (verbose >= 1)
    {
        printf("Ingress vlan_port_id = 0x%08x, in-port =%d\n", vlan_port.vlan_port_id, in_port);
    }

    /*
     * 2. create ingress router interface
     */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, in_vlan, 0, 0, my_mac_address, vrf);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,ing_qos_map_id, &eth_rif_structure.ing_qos_map_id);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,egr_qos_map_id, &eth_rif_structure.egr_qos_map_id);
    if (is_device_or_above(unit, JERICHO2))
    {
        eth_rif_structure.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        eth_rif_structure.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        eth_rif_structure.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_in = eth_rif_structure.l3_rif;

    /*
     * 3. create egress router interface
     */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, out_vlan, 0, 0, my_mac_address, vrf);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,ing_qos_map_id, &eth_rif_structure.ing_qos_map_id);
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,egr_qos_map_id, &eth_rif_structure.egr_qos_map_id);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf_out = eth_rif_structure.l3_rif;

    /*
     * 4. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &arp_encap_id, next_hop_mac, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 5. build FEC points to out-RIF and next-hop MAC address
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        fec = 20 * 1024 + 1;
    }
    rv = l3__egress_only_fec__create_inner(unit, fec, ing_intf_out, arp_encap_id, out_port, 0, 0, 0, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    if (verbose >= 1)
    {
        printf("created FEC-id =0x%08x\n", fec);
        printf("ARP encap-id = 0x%08x\n", arp_encap_id);
    }

    if (verbose >= 2)
    {
        printf("outRIF = 0x%08x out-port =%d\n", ing_intf_out, fec);
    }

    /*
     * 6. add host entry with FEC-ptr
     */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = encoded_fec;      /* FEC */

    /*
     * as encap id is valid (!=0), host will point to FEC + outlif
     */
    l3host.encap_id = 0;

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rv);
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }

    if (verbose >= 1)
    {
        print_host("add entry ", host, vrf);
        printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);
    }

    return rv;
}
