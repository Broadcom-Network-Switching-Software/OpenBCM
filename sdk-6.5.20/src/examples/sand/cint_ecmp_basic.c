/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ecmp_basic.c
 * cint
 * basic_ecmp_example(0,200,201);
 * exit;
 *
 * * ETH-RIF packet *
 * tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * * Received packets on unit 0 should be: *
 * * Source port: 0, Destination port: 0 *
 * * Data: 0x0000cd1d00123456789a81000000080045000035000000007f00fb45c08001017fffff02 *
 */

/*
 * NOF FECs in the ECMP group of the basic ECMP test.
 */
int NOF_OF_MEMBERS_ECMP_GROUP = 10;

/*
 * NOF FECs required for the ECMP TP testing.
 */
int NOF_OF_MEMBERS_ECMP_TP_GROUPS = 16;

struct cint_ecmp_basic_info_s
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
    int vrf;                                            /** VRF */
    bcm_if_t fec_id1;                                   /** FEC id 1*/
    bcm_if_t fec_id2;                                   /** FEC id 2*/
    bcm_if_t fec_id3;                                   /** FEC id 3*/
    bcm_if_t ecmp_id1;                                  /** ECMP interface id 1 */
    bcm_if_t ecmp_id2;                                  /** ECMP interface id 2 */
    bcm_if_t ecmp_id3;                                  /** ECMP interface id 3 */
    int new_outer_vlan;                                 /** Outer VLAN ID */
    int new_inner_vlan;                                 /** Inner VLAN ID */
    int outer_tpid;                                     /** Outer TPID */
    int inner_tpid;                                     /** Inner TPID */
    bcm_l3_ecmp_tunnel_priority_mode_t tp_mode;         /** tunnel priority mode */
};

cint_ecmp_basic_info_s cint_ecmp_info = {
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
    0x1021,
    /*
     * host
     */
    0x7fffff02 /** 127.255.255.2 */ ,
    /*
     * vrf
     */
    1,
    /*
     * fec_id1
     */
    0x4001,
    /*
     * fec_id2
     */
    0x10001,
    /*
     * fec_id3
     */
    0x18000,
    /*
     * ecmp_id1
     */
    50,
    /*
     * ecmp_id2
     */
    2150,
    /*
     * ecmp_id3
     */
    4097,
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
    /*
     * tp_mode
     */
    0
};

/*
 * Create an ECMP group
 * - ecmp_id - the ECMP ID
 * - intf_count - NOF members in the ECMP group
 * - intf_array - array of the group members
 * - flags - ECMP flags
 * - ecmp - the created ECMP group.
 */
int
l3__ecmp_create(
    int unit,
    int ecmp_id,
    int intf_count,
    bcm_if_t * intf_array,
    int flags,
    uint32 dynamic_mode,
    bcm_l3_egress_ecmp_t * ecmp)
{
    int rc;
    bcm_l3_egress_ecmp_t_init(ecmp);

    /*
     * ECMP properties
     */
    ecmp->max_paths = intf_count;
    ecmp->flags = flags;
    ecmp->ecmp_intf = ecmp_id;
    ecmp->dynamic_mode = dynamic_mode;
    /** If the ECMP group is in the extended range, add the needed flag. */
    if (ecmp_id >= 32768)
    {
        ecmp->ecmp_group_flags = BCM_L3_ECMP_EXTENDED;
    }
    /*
     * create an ECMP, containing the FECs entries 
     */
    rc = bcm_l3_egress_ecmp_create(unit, ecmp, intf_count, intf_array);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }

    return rc;
}

/*
 * Packets will be routed through different FEC from the in_port to the out_port and each FEC will have a different next hop MAC address.
 *
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff02
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
basic_ecmp_example(
    int unit,
    int in_port,
    int out_port,
    int is_consistent,
    int ecmp_id)
{
    int rv;
    int iter;
    bcm_if_t intf_array[NOF_OF_MEMBERS_ECMP_GROUP];
    bcm_gport_t gport;
    bcm_l3_egress_ecmp_t ecmp;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    cint_ecmp_info.in_port = in_port;
    cint_ecmp_info.out_port = out_port;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_ecmp_info.in_port, cint_ecmp_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_ecmp_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_port\n");
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_in, cint_ecmp_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ecmp_info.intf_in;
    ingr_itf.vrf = cint_ecmp_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * 5. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_out, cint_ecmp_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_ecmp_info.out_port);

    /*
     * 5. Create FECs for the ECMP group
 */
    for (iter = 0; iter < NOF_OF_MEMBERS_ECMP_GROUP; iter++)
    {
        /*
         * 5.1 Create ARP and set its properties
         */
        rv = l3__egress_only_encap__create(unit, 0, &cint_ecmp_info.encap_id1, cint_ecmp_info.arp_next_hop_mac, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }



        /*
         * 5.1 Create FEC and set its properties
         */
        sand_utils_l3_fec_s fec_structure;
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination = gport;
        fec_structure.tunnel_gport = cint_ecmp_info.intf_out;
        fec_structure.tunnel_gport2 = cint_ecmp_info.encap_id1;
        fec_structure.fec_id = cint_ecmp_info.fec_id3;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }

        {
            
            cint_ecmp_info.encap_id1++;
            rv = l3__egress_only_encap__create(unit, 0, &cint_ecmp_info.encap_id1, cint_ecmp_info.arp_next_hop_mac, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object ARP only\n");
                return rv;
            }

        }


        /*
         * Place the new FEC into the interfaces array of the ECMP
         * Increment the FEC number ad the encap number for the next FEC.
         * Set a different next hop MAC to identify different FEC selection.
         */
        intf_array[iter] = cint_ecmp_info.fec_id3;
        cint_ecmp_info.fec_id3++;
        cint_ecmp_info.encap_id1++;
        cint_ecmp_info.arp_next_hop_mac[5]++;
    }

    /*
     * 6. Create an ECMP group
     */
    if (is_consistent == 0)
    {
        rv = l3__ecmp_create(unit, ecmp_id, NOF_OF_MEMBERS_ECMP_GROUP, intf_array, BCM_L3_WITH_ID,
                             BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    }
    else
    {
        rv = l3__ecmp_create(unit, ecmp_id, NOF_OF_MEMBERS_ECMP_GROUP, intf_array, BCM_L3_WITH_ID,
                             BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT, &ecmp);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, create ECMP group\n");
        return rv;
    }

    /*
     * 7. Add host entry
     */
    rv = add_host_ipv4(unit, cint_ecmp_info.host, cint_ecmp_info.vrf, ecmp_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host, \n");
        return rv;
    }

    return rv;
}

/*
 * basic ECMP tunnel priority setup.
 * The NOF priorities is set by the cint_ecmp_info.tp_mode
 */
int
basic_ecmp_tp_example(
    int unit,
    int in_port,
    int out_port,
    int ecmp_id)
{

    int rv;
    int iter;
    int dscp;
    int flags_qos = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
    bcm_if_t intf_array[NOF_OF_MEMBERS_ECMP_TP_GROUPS];
    bcm_gport_t gport;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_qos_map_t l3_ing_map;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_qos_map_t_init(&l3_ing_map);
    cint_ecmp_info.in_port = in_port;
    cint_ecmp_info.out_port = out_port;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_ecmp_info.in_port, cint_ecmp_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_ecmp_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_port\n");
        return rv;
    }

    /*
    * 3. Set the QOS map, and match each DSCP value to a traffic class value.
    * DSCP 0 will get TC 0, DSCP 1 will get TC 1...
    */
     rv = dnx_qos_ingress_phb_map(unit, 0, 0);
     if (rv != BCM_E_NONE)
     {
         printf("Error, out_port_set out_port\n");
         return rv;
     }

     for(dscp = 0; dscp < 8; dscp++)
     {
         l3_ing_map.dscp             = dscp;
         l3_ing_map.int_pri          = dscp;
         rv = bcm_qos_map_add(unit, flags_qos, &l3_ing_map, ing_opcode_id);
         if (rv != BCM_E_NONE) {
             print rv;
         }
     }

    /*
    * 4. Create ETH-RIF and set its properties
    */
    rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_in,cint_ecmp_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }


    /*
     * 5. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ecmp_info.intf_in;
    ingr_itf.vrf = cint_ecmp_info.vrf;
    ingr_itf.qos_map = ing_phb_map_id;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * 6. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_out, cint_ecmp_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_ecmp_info.out_port);

    /*
     * 7. Create FECs for the ECMP group
 */
    for (iter = 0; iter < NOF_OF_MEMBERS_ECMP_TP_GROUPS; iter++)
    {
        /*
         * 7.1 Create ARP and set its properties
         */
        rv = l3__egress_only_encap__create(unit, 0, &cint_ecmp_info.encap_id1, cint_ecmp_info.arp_next_hop_mac, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only\n");
            return rv;
        }

        /*
         * 7.2 Create FEC and set its properties
         */
        sand_utils_l3_fec_s fec_structure;
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination = gport;
        fec_structure.tunnel_gport = cint_ecmp_info.intf_out;
        fec_structure.tunnel_gport2 = cint_ecmp_info.encap_id1;
        fec_structure.fec_id = cint_ecmp_info.fec_id3;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }

        {
            /*
             * Currently in adapter consecutive even and odd LIF entries get the even entry payload.
             * This allocation is a to create odd LIF entries but don't use them so we can be sure what is the ARP payload
             * that will be received.
             */
            cint_ecmp_info.encap_id1++;
            rv = l3__egress_only_encap__create(unit, 0, &cint_ecmp_info.encap_id1, cint_ecmp_info.arp_next_hop_mac, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object ARP only\n");
                return rv;
            }

        }


        /*
         * Place the new FEC into the interfaces array of the ECMP
         * Increment the FEC number ad the encap number for the next FEC.
         * Set a different next hop MAC to identify different FEC selection.
         */
        intf_array[iter] = cint_ecmp_info.fec_id3;
        cint_ecmp_info.fec_id3++;
        cint_ecmp_info.encap_id1++;
        cint_ecmp_info.arp_next_hop_mac[5]++;
    }




    /*
     * 8. Create tunnel priority map for the ECMP group
     */
    int traffic_class_max_size = 8;/** The traffic class size is 3 bits */
    int nof_tables = 1 << cint_ecmp_info.tp_mode;
    int tp_iter;
    int tp_range_per_size_index = traffic_class_max_size / nof_tables;
    bcm_l3_ecmp_tunnel_priority_map_info_t map_info;
    map_info.l3_flags = 0;
    rv = bcm_l3_ecmp_tunnel_priority_map_create(unit,&map_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ecmp_tunnel_priority_map_create\n");
        return rv;
    }

    for(tp_iter = 0; tp_iter < traffic_class_max_size; tp_iter++)
    {
        map_info.tunnel_priority = tp_iter;
        map_info.index = tp_iter / tp_range_per_size_index;
        rv = bcm_l3_ecmp_tunnel_priority_map_set(unit,&map_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ecmp_tunnel_priority_map_create\n");
            return rv;
        }
    }

    /*
     * 9. Create an TP ECMP group
     */
    bcm_l3_egress_ecmp_t_init(ecmp);
    ecmp.max_paths = NOF_OF_MEMBERS_ECMP_TP_GROUPS;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_id;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    ecmp.tunnel_priority.index = 0;
    ecmp.tunnel_priority.map_profile = map_info.map_profile;
    ecmp.tunnel_priority.mode = cint_ecmp_info.tp_mode;
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 1, &intf_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
        return rv;
    }

    bcm_if_t index_intf_array[NOF_OF_MEMBERS_ECMP_TP_GROUPS];
    int nof_interfaces_per_index = NOF_OF_MEMBERS_ECMP_TP_GROUPS / nof_tables;
    int index_iter;

    /*
     * 10. Update each one of the ECMP group TP tables with different FEC members
     */
    for(index_iter = 0;index_iter < nof_tables;index_iter++)
    {

        for (iter = 0; iter < nof_interfaces_per_index; iter++)
        {
            index_intf_array[iter] = intf_array[index_iter*nof_interfaces_per_index + iter];
        }

        ecmp.tunnel_priority.index = index_iter;
        rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit,&ecmp,nof_interfaces_per_index,index_intf_array);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_ecmp_table_set failed: %d \n", rv);
            return rv;
        }
    }

    /*
     * 11. Add host entry
     */
    rv = add_host_ipv4(unit, cint_ecmp_info.host, cint_ecmp_info.vrf, ecmp_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host, \n");
        return rv;
    }

    return rv;

}


int
ecmp_consistent_hashing_remove_member(
    int unit,
    int ecmp_id,
    int fec_member)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t ecmp;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_id;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;

    rv = bcm_l3_egress_ecmp_delete(unit, &ecmp, fec_member);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_egress_ecmp_delete, \n");
        return rv;
    }
    return rv;
}

int
ecmp_consistent_hashing_add_member(
    int unit,
    int ecmp_id,
    int fec_member)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t ecmp;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_id;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    rv = bcm_l3_egress_ecmp_add(unit, &ecmp, fec_member);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function bcm_l3_egress_ecmp_add, \n");
        return rv;
    }
    return rv;
}

/*
 * hierarchy ECMPS in the group.
 */
int NOF_1ST_HIER_FECS = 5;
/*
 * NOF FECs in each 2nd hierarchy ECMP.
 */
int NOF_2ND_HIER_FECS = 5;
/*
 * NOF FECs in each 3rd hierarchy ECMP.
 */
int NOF_3RD_HIER_FECS = 5;
/*
 * Test Scenario
 * This test checks the hierarchical ECMP (two hierarchies), the first hierarchy level sets the out-RIF and the second one the ARP (DA).
 * When changing the header parameters we're expecting that both the out-RIF and the DA of the outgoing packets will be modified
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ecmp_basic.c
 * cint
 * basic_ecmp_hierarchical_example(0,200,201);
 *
 *
 *                                                                                                            +----> FEC_000 (3rd hier)
 *                                                                                      +-> ECMP_00(3rd hier) |---->    .
 *                                                                                      |          .          +----> FEC_00P (3rd hier)
 *                                                               +----> FEC_00(2nd hier)+          .                    .
 *                                       +----> ECMP_0(2nd hier) |---->                            .          +----> FEC_0M0 (3rd hier)
 *                 +---->FEC_0 (1st hier)+             .         +----> FEC_0M(2nd hier)+-> ECMP_0M 3rd hier) |---->    .
 *                 |          .                        .                                           .          +----> FEC_0MP (3rd hier)
 *                 +---->     .                        .                                           .                    .
 * ECMP (1st hier) |          .                        .                                           .                    .
 *                 +---->     .                        .                                           .                    .
 *                 |          .                        .                                                      +----> FEC_N00 (3rd hier)
 *                 +---->FEC_N (1st hier)+             .         +----> FEC_N0(2nd hier)+-> ECMP_N0(3rd hier) |---->    .
 *                                       +----> ECMP_N(2nd hier) |---->                            .          +----> FEC_N0P (3rd hier)
 *                                                               +----> FEC_NM(2nd hier)+          .                    .
 *                                                                                      |          .          +----> FEC_NM0 (3rd hier)
 *                    (FECs from 0 to N)                                                +-> ECMP_NM(3rd hier) |---->    .
 *                                                           (FECs from 0 to M for each ECMP group)           +----> FEC_NMP (3rd hier)
 *                                                                                                      (FECs from 0 to P for each ECMP group)
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int
basic_ecmp_hierarchical_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int iter_1st;
    int iter_2nd;
    int iter_3rd;
    int rif = 70;

    bcm_gport_t gport;
    bcm_gport_t gport_ecmp;
    bcm_gport_t lif_as_gport = 0;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_egress_ecmp_t ecmp1;
	sand_utils_l3_fec_s fec_structure;

    l3_ingress_intf ingress_itf;
    bcm_if_t fec_array_hier1[NOF_1ST_HIER_FECS];
    bcm_if_t fec_array_hier2[NOF_2ND_HIER_FECS];
    bcm_if_t fec_array_hier3[NOF_3RD_HIER_FECS];

    cint_ecmp_info.in_port = in_port;
    cint_ecmp_info.out_port = out_port;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ecmp_info.fec_id1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_ecmp_info.fec_id2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, &cint_ecmp_info.fec_id3);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_ecmp_info.in_port, cint_ecmp_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_ecmp_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_port\n");
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_in, cint_ecmp_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_itf);
    ingress_itf.intf_id = cint_ecmp_info.intf_in;
    ingress_itf.vrf = cint_ecmp_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set intf_out\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_ecmp_info.out_port);

    /*
     * This is a nested loop, the outer loop creates a first level ECMP group that provides (using the first level FEC)
     * and out-RIF and an inner loop that provide in the second hierarchy level an ARP.
     */
    for (iter_1st = 0; iter_1st < NOF_1ST_HIER_FECS; iter_1st++)
    {
        rv = intf_eth_rif_create(unit, cint_ecmp_info.intf_out, cint_ecmp_info.intf_out_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create intf_out\n");
            return rv;
        }

        /*
         * Loop over the creation of FECs in different hierarchies.
         */
        for (iter_2nd = 0; iter_2nd < NOF_2ND_HIER_FECS; iter_2nd++)
        {
            /*
             * 5. Create ARP and set its properties for each 3H FEC
             */
            rv = l3__egress_only_encap__create(unit, 0, &cint_ecmp_info.encap_id1, cint_ecmp_info.arp_next_hop_mac,
                                               cint_ecmp_info.intf_out);
            if (rv != BCM_E_NONE)
            {
                printf("Error, l3__egress_only_encap__create - ARP only, error = (%d), encap_id = %04x \n", rv,
                       cint_ecmp_info.encap_id1);
                return rv;
            }
            /*
             * 6. Create 3rd hierarchy FECs with encap_id_2 and gport pointing to the out_port
             */
            for (iter_3rd = 0; iter_3rd < NOF_3RD_HIER_FECS; iter_3rd++)
            {
                sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
                fec_structure.destination = gport;
                fec_structure.tunnel_gport = rif;
                fec_structure.tunnel_gport2 = cint_ecmp_info.encap_id2;
                fec_structure.fec_id = cint_ecmp_info.fec_id3;
                fec_structure.flags = BCM_L3_3RD_HIERARCHY;
                fec_structure.allocation_flags = BCM_L3_WITH_ID;
                rv = sand_utils_l3_fec_create(unit, &fec_structure);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, create egress object FEC only\n");
                    return rv;
                }

                fec_array_hier3[iter_3rd] = cint_ecmp_info.fec_id3;
                rif++;
                cint_ecmp_info.fec_id3++;
            }
            /*
             * 7. Create ECMP group in 3rd hieararchy with the list of 3H FECs.
             * Until ECMP allocation manager is ready, ecmp is created with WITH_ID flag
             */
            rv = l3__ecmp_create(unit, cint_ecmp_info.ecmp_id3, NOF_3RD_HIER_FECS, fec_array_hier3,
                                 BCM_L3_WITH_ID | BCM_L3_3RD_HIERARCHY, BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp1);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in l3__ecmp_create 3rd hierarchy\n");
                return rv;
            }

            /*
             * 8. Create second hierarchy FECs pointing to the ECMP group in 3rd hierarchy
             */
            BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_ecmp, ecmp1.ecmp_intf);
            printf("FEC_2 = 0x%x\n", cint_ecmp_info.fec_id2);
            sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
            fec_structure.destination = gport_ecmp;
            fec_structure.tunnel_gport = 0;
            fec_structure.tunnel_gport2 = cint_ecmp_info.encap_id1;
            fec_structure.fec_id = cint_ecmp_info.fec_id2;
            fec_structure.flags = BCM_L3_2ND_HIERARCHY;
            fec_structure.allocation_flags = BCM_L3_WITH_ID;
            rv = sand_utils_l3_fec_create(unit, &fec_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, create egress object FEC only\n");
                return rv;
            }

            fec_array_hier2[iter_2nd] = cint_ecmp_info.fec_id2;
            cint_ecmp_info.fec_id2++;
            cint_ecmp_info.encap_id1++;
            cint_ecmp_info.arp_next_hop_mac[5]++;
            cint_ecmp_info.ecmp_id3++;
        }

        /*
         * 9. Create the second hierarchy ECMP group with the list of FECs in hierarchy 2.
         */
        rv = l3__ecmp_create(unit, cint_ecmp_info.ecmp_id2, NOF_2ND_HIER_FECS, fec_array_hier2,
                             BCM_L3_WITH_ID | BCM_L3_2ND_HIERARCHY, BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create ECMP group 2nd hierarchy\n");
            return rv;
        }

        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_ecmp, ecmp.ecmp_intf);
        /*
         * 10. Create first level FECs that point to a second level ECMP group
         */
        printf("FEC_1 = 0x%x\n", cint_ecmp_info.fec_id1);
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination = gport_ecmp;
        fec_structure.tunnel_gport = cint_ecmp_info.intf_out;
        fec_structure.tunnel_gport2 = 0;
        fec_structure.fec_id = cint_ecmp_info.fec_id1;
        fec_structure.allocation_flags = BCM_L3_WITH_ID;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
            return rv;
        }

        fec_array_hier1[iter_1st] = cint_ecmp_info.fec_id1;
        cint_ecmp_info.fec_id1++;
        cint_ecmp_info.intf_out++;
        cint_ecmp_info.ecmp_id2++;
    }

    /*
     * 11. Create AC-Ptr for 3rd-hier FECs and set its properties
     */
    rv = out_vlan_port_intf_set(unit, cint_ecmp_info.out_port, cint_ecmp_info.encap_id2, &lif_as_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_vlan_port_intf_set out_port\n");
        return rv;
    }

    /*
     * Add VLAN editing for encap stage (using UNTAG tag_struct - 0)
     */
    rv = vlan_translate_ive_eve_translation_set(unit, lif_as_gport, cint_ecmp_info.outer_tpid,
                                                cint_ecmp_info.inner_tpid, bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_ecmp_info.new_outer_vlan, cint_ecmp_info.new_inner_vlan, 1, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_ive_eve_translation_set\n");
        return rv;
    }

    /*
     * 12. Create first hierarchy ECMP group with the list of FECs in 1st hierarchy
     */
    rv = l3__ecmp_create(unit, cint_ecmp_info.ecmp_id1, NOF_1ST_HIER_FECS, fec_array_hier1, BCM_L3_WITH_ID,
                         BCM_L3_ECMP_DYNAMIC_MODE_DISABLED, &ecmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create ECMP group 1st hierarchy\n");
        return rv;
    }

    /*
     * 13. Add host entry with the 1H ECMP group as result
     */
    rv = add_host_ipv4(unit, cint_ecmp_info.host, cint_ecmp_info.vrf, ecmp.ecmp_intf, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host, \n");
        return rv;
    }

    return rv;
}
