/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_mpls_ecmp.c
 * Purpose: Example of two levels MPLS-ECMP configuration.
 *
 *   Incoming UDP Packet with dip 56.56.56.56 and random sip and source port
 *      * * *
 *    *       *                                          Inner outgoing
 *    *       *                            * * * * * * * * * * *   MPLS label (0x1000)
 *    *   |   *                          *                       *
 *      * | *                          *  0        * * *           *
 *        |                           *  0       *  101  *          *
 *        |                          *  0        *      ------------------------->
 *        |                         *  1         /       *            *
 *        |   +----+               *            /  * * *               *
 *        |   |ECMP|               *  +----+   /            * * *      *
 *        \-->| 1  |----------------->|ECMP|----------------      *    *       Outer outgoing
 *            +----+               *  |1_2 |   \          *  102  *    *          MPLS label (0x101 / 0x102 / 0x103)
 *               | Hashing         *  +----+    \         *     ----------------->
 *               | configured      *             \ * *      * * *      *
 *               | seperatly        *          *  \    *              *
 *               | per  ECMP         *         *      --------------------------->
 *               | level              *        *  103  *            *
 *               |                     *         * * *             *
 *               |                       *                       *
 *               |                         * * * * * * * * * * *
 *               |
 *               |                                      Inner outgoing
 *               |                         * * * * * * * * * * *   MPLS label (0x2000)
 *               |                       *                       *
 *               |                      *  0        * * *           *
 *               |                     *  0       *  201  *          *
 *               |                    *  0        *      ------------------------->
 *               |                   *  2         /       *            *
 *               |                  *            /  * * *               *
 *               |                  *  +----+   /            * * *      *
 *               \-------------------->|ECMP|----------------      *    *       Outer outgoing
 *                                  *  |2_2 |   \          *  202  *    *          MPLS label (0x201 / 0x202 / 0x203)
 *                                  *  +----+    \         *     ----------------->
 *                                   *             \ * *      * * *      *
 *                                    *          *  \    *              *
 *                                     *         *      --------------------------->
 *                                      *        *  203  *            *
 *                                       *         * * *             *
 *                                        *                       *
 *                                         * * * * * * * * * * *
 *
 * Explanation:
 *  Incoming MPLS over Ethernet packets coming from in_port, with vlan 100
 *  will be directed to ECMP 1 which will select next FEC by hash results.
 *  next FEC will point to an MPLS tunnel and will forward to another ECMP which will select next FEC according to hash result.
 *  This FEC will point to another MPLS tunnel and will forward to physical out_port.
 *
 * Packets:
 *  Incoming
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *  | Ethernet                                          | Payload             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *  | DA                | SA | TPID   | PCP | DEI | VID | SIP    | .. | SPORT |
 *  | 00:00:00:01:01:01 |    | 0x8100 |     |     | 100 | random | .. | ramdom|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *
 *  Outgoing:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Ethernet                                          | MPLS                    | MPLS                    | Payload |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | DA                | SA | TPID   | PCP | DEI | VID | Label  | TC | BOS | TTL | Label  | TC | BOS | TTL |         |
 *  | 00:00:00:08:08:08 |    | 0x8100 |     |     | 200 | 0x101  |    | 0   |     | 0x1000 |    | 0   |     |         |
 *  |                   |    |        |     |     |     | 0x102  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x103  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x201  |    |     |     | 0x2000 |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x202  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x203  |    |     |     |        |    |     |     |         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_mpls_ecmp.c
 * cint
 * mpls_dual_ecmp_main(0,200,203,1);
 * exit;
 *
 *  send packet
 * tx 1 psrc=200 data=0x0000000101010000000002228100006408004500003d0000000080117065534308323737379f03e8000000290000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be:
 *  Source port: 0, Destination port: 0
 *  Data: 0x000000080808000000010102810000c888470010201e010001144500003d000000007f117165534308323737379f03e8000000290000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

struct mpls_ecmp_info_s {
    int in_vlan;
    int in_label;
	uint32 host;
    uint32 route;
    int out_label1;
    int out_label1_1;
    int out_label1_2;
    int out_label1_3;
    int out_label2;
    int out_label2_1;
    int out_label2_2;
    int out_label2_3;
    int out_vlan;
    uint8 my_mac1[6];
    uint8 my_mac2[6];
    uint8 next_hop_mac[6];
};

mpls_ecmp_info_s mpls_ecmp_info_1;

int mpls_ecmp_init() {
    uint8 my_mac1[6] = {0, 0, 0, 1, 1, 1};
    uint8 my_mac2[6] = {0, 0, 0, 1, 1, 2};
    uint8 next_hop_mac[6] = {0, 0, 0, 8, 8, 8};

    mpls_ecmp_info_1.in_vlan = 100;
	mpls_ecmp_info_1.host = 0x38383838;/*56.56.56.56*/
    mpls_ecmp_info_1.route = 0x37373737;/*55.55.55.55*/
    mpls_ecmp_info_1.out_label1 = 0x1000;
    mpls_ecmp_info_1.out_label1_1 = 0x101;
    mpls_ecmp_info_1.out_label1_2 = 0x102;
    mpls_ecmp_info_1.out_label1_3 = 0x103;
    mpls_ecmp_info_1.out_label2 = 0x2000;
    mpls_ecmp_info_1.out_label2_1 = 0x201;
    mpls_ecmp_info_1.out_label2_2 = 0x202;
    mpls_ecmp_info_1.out_label2_3 = 0x203;
    mpls_ecmp_info_1.out_vlan = 200;
    sal_memcpy(mpls_ecmp_info_1.my_mac1, my_mac1, 6);
    sal_memcpy(mpls_ecmp_info_1.my_mac2, my_mac2, 6);
    sal_memcpy(mpls_ecmp_info_1.next_hop_mac, next_hop_mac, 6);

    return 0;
}

/* Create MPLS tunnel initiator */
int mpls_ecmp_tunnel_initiator_create(int unit, int level, int label, int out_interface, int* tunnel_id) {
    int rc;

    bcm_mpls_egress_label_t label_array[1];
    label_array[0].exp = 0;
    label_array[0].label = label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    if (level == 2) {
        label_array[0].ttl = 20;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].encap_access = bcmEncapAccessTunnel1;
    } else {
        label_array[0].ttl = 30;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        label_array[0].encap_access = bcmEncapAccessTunnel2;
    }

    rc = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create Label = 0x%x\n", label_array[0].label);
        return rc;
    }

    *tunnel_id = label_array[0].tunnel_id;

    printf("MPLS tunnel created. Label = 0x%x, Tunnel id = 0x%x\n", label_array[0].label, *tunnel_id);

    return rc;
}

/* Create first FEC for hierarchical FEC (ingress only) */
int mpls_ecmp_first_fec_create(int unit, int tunnel_id,
                               bcm_failover_t failover_id,
                               bcm_gport_t out_gport, bcm_if_t* fec_id)
{
    int rc;
    dnx_utils_l3_fec_s l3_fec;

    dnx_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
    l3_fec.tunnel_gport = tunnel_id;
    l3_fec.destination= out_gport;
    l3_fec.failover_id = failover_id;
    l3_fec.failover_if_id = 0;

    rc = dnx_utils_l3_fec_create(unit, &l3_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, dnx_utils_l3_fec_create\n");
        return rc;
    }

    *fec_id = l3_fec.l3eg_fec_id;
    printf("FEC created. ID = 0x%x, for tunnel id = 0x%x\n", *fec_id, tunnel_id);

    return rc;
}


/* Create cascaded FEC */
int mpls_ecmp_cascaded_fec_create(int unit, int out_interface, int out_port, int out_vlan, bcm_mac_t next_hop_mac, bcm_if_t* fec_id) {
    int rc;
    int encap_id = 0;
    int gport =0;
    dnx_utils_l3_fec_s l3_fec;
    dnx_utils_l3_arp_s l3_arp;

    dnx_utils_l3_arp_s_init(unit, 0x0, &l3_arp);
    l3_arp.next_hop_mac_addr= next_hop_mac;
    l3_arp.eth_rif = out_vlan;
    l3_arp.intf_id = out_interface;
    rc = dnx_utils_l3_arp_create(unit, &l3_arp);
    if (rc != BCM_E_NONE) {
        printf("Error, dnx_utils_l3_arp_create\n");
        return rc;
    }
    encap_id = l3_arp.arp_id;

    BCM_GPORT_LOCAL_SET(gport, out_port);
    dnx_utils_l3_fec_s_init(unit, 0x0, &l3_fec);
    l3_fec.tunnel_gport = out_interface;
    l3_fec.tunnel_gport2 = encap_id;
    l3_fec.destination= gport;
    l3_fec.flags= BCM_L3_CASCADED;
    rc = dnx_utils_l3_fec_create(unit, &l3_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, dnx_utils_l3_fec_create\n");
        return rc;
    }

    *fec_id = l3_fec.l3eg_fec_id;
    printf("Cascaded FEC created. ID = 0x%x, for tunnel id = 0x%x\n", *fec_id, out_interface);

    return rc;
}

/* Dual level ECMP with MPLS
   If slb_resolve is set- ecmp is resolved using Flexible-Hashing hw, otherwise using configured LB*/
int mpls_dual_ecmp_main(int unit, int in_port, int out_port, int slb_resolve) {
    int rc;
	uint32 reg32_val;
    l3_ingress_intf ingress_rif;
    int vrf =1;

    /* Init default settings */
    mpls_ecmp_init();

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rc = in_port_intf_set(unit, in_port, mpls_ecmp_info_1.in_vlan);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port, mpls_ecmp_info_1.in_vlan,rc);
        return rc;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rc = out_port_set(unit, out_port);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rc);
        return rc;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rc = intf_eth_rif_create(unit, mpls_ecmp_info_1.in_vlan, mpls_ecmp_info_1.my_mac1);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, mpls_ecmp_info_1.in_vlan, rc);
        return rc;
    }

    rc = intf_eth_rif_create(unit, mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.my_mac2);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, mpls_ecmp_info_1.out_vlan, rc);
        return rc;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = mpls_ecmp_info_1.in_vlan;
    rc = intf_ingress_rif_set(unit, &ingress_rif);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, mpls_ecmp_info_1.in_vlan, vrf, rc);
        return rc;
    }

    /* Creating two level-2 MPLS tunnels for outgoing packets */
    int tunnel_id_1, tunnel_id_2;
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label1, 0, &tunnel_id_1);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label2, 0, &tunnel_id_2);

    /* Creating six level-1 MPLS tunnels for outgoing packets */
    int tunnel_id_1_1, tunnel_id_1_2, tunnel_id_1_3, tunnel_id_2_1, tunnel_id_2_2, tunnel_id_2_3;
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_1, mpls_ecmp_info_1.out_vlan, &tunnel_id_1_1);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_2, mpls_ecmp_info_1.out_vlan, &tunnel_id_1_2);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_3, mpls_ecmp_info_1.out_vlan, &tunnel_id_1_3);

    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_1, mpls_ecmp_info_1.out_vlan, &tunnel_id_2_1);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_2, mpls_ecmp_info_1.out_vlan, &tunnel_id_2_2);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_3, mpls_ecmp_info_1.out_vlan, &tunnel_id_2_3);

    /* Creating level-1 FEC for each level-1 MPLS tunnel */
    bcm_if_t fec_id1_1, fec_id1_2, fec_id1_3, fec_id2_1, fec_id2_2, fec_id2_3;

    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_1, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id1_1);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_2, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id1_2);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_3, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id1_3);

    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_1, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id2_1);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_2, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id2_2);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_3, out_port,
                                  mpls_ecmp_info_1.out_vlan, mpls_ecmp_info_1.next_hop_mac, &fec_id2_3);

    /* Creating two level-2 ECMPs each to balance between two level-2 MPLS tunnels */
    int interface_count = 3;
    bcm_if_t interfaces1_1[interface_count];
    bcm_if_t interfaces1_2[interface_count];

    interfaces1_1[0] = fec_id1_1;
    interfaces1_1[1] = fec_id1_2;
    interfaces1_1[2] = fec_id1_3;

    interfaces1_2[0] = fec_id2_1;
    interfaces1_2[1] = fec_id2_2;
    interfaces1_2[2] = fec_id2_3;

    bcm_l3_egress_ecmp_t ecmp1_1;
    bcm_l3_egress_ecmp_t_init(&ecmp1_1);
    ecmp1_1.max_paths = interface_count;
    if (slb_resolve) {
        ecmp1_1.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }
    ecmp1_1.flags |= BCM_L3_CASCADED;

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1_1, interface_count, interfaces1_1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-2 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x, 0x%x\n", ecmp1_1.ecmp_intf, interfaces1_1[0], interfaces1_1[1], interfaces1_1[2]);

    bcm_l3_egress_ecmp_t ecmp1_2;
    bcm_l3_egress_ecmp_t_init(&ecmp1_2);
    ecmp1_2.max_paths = interface_count;
    if (slb_resolve) {
        ecmp1_2.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }
    ecmp1_2.flags |= BCM_L3_CASCADED;

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1_2, interface_count, interfaces1_2);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-2 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x, 0x%x\n", ecmp1_2.ecmp_intf, interfaces1_2[0], interfaces1_2[1], interfaces1_2[2]);

    bcm_if_t fec_id1;
    bcm_if_t fec_id2;
    bcm_gport_t out_gport;

    /* level-1 MPLS tunnel will be chosen by ECMP1_1 or ECMP1_2*/
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_gport, ecmp1_1.ecmp_intf);
    rc = mpls_ecmp_first_fec_create(unit, tunnel_id_1, 0, out_gport, &fec_id1);
    if (rc != BCM_E_NONE) {
        printf("Error, mpls_ecmp_first_fec_create\n");
        return rc;
    }


    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_gport, ecmp1_2.ecmp_intf);
    rc = mpls_ecmp_first_fec_create(unit, tunnel_id_2, 0, out_gport, &fec_id2);
    if (rc != BCM_E_NONE) {
        printf("Error, mpls_ecmp_first_fec_create\n");
        return rc;
    }

    /* Creating level-1 ECMP to balance between the two level-2 MPLS tunnels */
    interface_count = 2;
    bcm_if_t interfaces1[interface_count];
    interfaces1[0] = fec_id1;
    interfaces1[1] = fec_id2;

    bcm_l3_egress_ecmp_t ecmp1;
    bcm_l3_egress_ecmp_t_init(&ecmp1);
    ecmp1.max_paths = interface_count;
    if (slb_resolve) {
        ecmp1.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1, interface_count, interfaces1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-1 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x\n", ecmp1.ecmp_intf, interfaces1[0], interfaces1[1]);

    /* add host entry and point to the level-1 ECMP */
    bcm_l3_host_t l3host;
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = mpls_ecmp_info_1.host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = ecmp1.ecmp_intf;

    
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host(route)_add failed: %x \n", rc);
        return rc;
    }

    /*Add route entry*/
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = mpls_ecmp_info_1.route;
    l3rt.l3a_ip_mask = 0xFFFFFF00;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = ecmp1.ecmp_intf;
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %x \n", rc);
    }

    return rc;
}


