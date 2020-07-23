/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_ecmp.c
 * Purpose: Example of two levels MPLS-ECMP configuration.
 *
 *   Incoming UDP Packet with dip 172.172.172.172 and random sip and source port
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
 *  Incoming MPLS over Ethernet packets coming from in_port, vlan 100 with a label of 0x5000
 *  will be directed to ECMP 1 which will select next FEC by hash results.
 *  next FEC will point to an MPLS tunnel and will forward to another ECMP which will select next FEC according to hash result.
 *  This FEC will point to another MPLS tunnel and will forward to physical out_port.
 *
 * Packets:
 *  Incoming
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *  | Ethernet                                          | MPLS                    | Payload             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *  | DA                | SA | TPID   | PCP | DEI | VID | Label  | TC | BOS | TTL | SIP    | .. | SPORT |
 *  | 00:00:00:01:01:01 |    | 0x8100 |     |     | 100 | 0x5000 |    | 1   |     | random | .. | ramdom|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++++++++++++
 *
 *  Outgoing:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Ethernet                                          | MPLS                    | MPLS                    | MPLS                    | Payload |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | DA                | SA | TPID   | PCP | DEI | VID | Label  | TC | BOS | TTL | Label  | TC | BOS | TTL | Label  | TC | BOS | TTL |         |
 *  | 00:00:00:08:08:08 |    | 0x8100 |     |     | 200 | 0x101  |    | 0   |     | 0x1000 |    | 0   |     | 0x5000 |    | 1   |     |         |
 *  |                   |    |        |     |     |     | 0x102  |    |     |     |        |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x103  |    |     |     |        |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x201  |    |     |     | 0x2000 |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x202  |    |     |     |        |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x203  |    |     |     |        |    |     |     |        |    |     |     |         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * To Activate Above Settings Run:
 * cd ../../../../src/examples/dpp/
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_vlan.c
 * cint utility/cint_utils_l3.c
 * cint cint_ip_route.c
 * cint validate/cint_hw_access.c  
 * cint cint_mpls_ecmp.c
 * cint cint_stateful_load_balancing.c
 * cint
 *      cint> mpls_dual_ecmp_main(0,200,201,1);
 * print flexible_two_keys_sport_sip(0, BCM_HASH_CONFIG_CRC16_0x8423);
 * exit;
 */

struct mpls_ecmp_info_s {
    int in_vlan;
    int in_label;
	uint32 host;
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
	mpls_ecmp_info_1.host = 0x37373737;/*55.55.55.55*/
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
	int is_qax;

    rc = is_qumran_ax_only(unit, &is_qax);
    if (rc != BCM_E_NONE) {
        printf("Error, is_qumran_ax_only()\n");
        return rc;
    }

    bcm_mpls_egress_label_t label_array[1];
    label_array[0].exp = 0;
    label_array[0].label = label;
    if (is_device_or_above(unit, JERICHO2)) {
       label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
       label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    } else {
       label_array[0].flags = BCM_MPLS_EGRESS_LABEL_ACTION_VALID|BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
       label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;    
       label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH; 
    }
    if (level == 2) {
        label_array[0].ttl = 20;
        if (is_device_or_above(unit, JERICHO2)) {
           label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
           label_array[0].encap_access = bcmEncapAccessTunnel1;
        } else
        {
           BCM_L3_ITF_SET(label_array[0].l3_intf_id, BCM_L3_ITF_TYPE_LIF, 0); /* Crucial part - Otherwise MPLS tunnels will compete in the same phase */
        }
    } else {
        label_array[0].ttl = 30;
        if (is_device_or_above(unit, JERICHO2)) {
           label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
           label_array[0].encap_access = bcmEncapAccessTunnel2;
        } else
        {  
           label_array[0].l3_intf_id = out_interface;
        }
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
    /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
     * In case we are using hierarchical FEC, the first FEC must be protected.
     * This was fixed in Jericho B0.
     */
    create_l3_egress_s l3eg_fec;
    l3eg_fec.out_tunnel_or_rif = tunnel_id; 
    l3eg_fec.out_gport = out_gport;
    l3eg_fec.failover_id = failover_id; 
    l3eg_fec.failover_if_id = 0;
    sal_memset(l3eg_fec.next_hop_mac_addr, 0, 6);

    if ((is_device_or_above(unit, JERICHO2) && 
           (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0))
        || !is_device_or_above(unit, JERICHO2)) {
        l3eg_fec.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    } 

    if (!is_device_or_above(unit,JERICHO_B0)) {
        rc = l3__egress_only_fec__create(unit,&l3eg_fec);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }
        l3eg_fec.failover_if_id = l3eg_fec.fec_id;
        l3eg_fec.arp_encap_id = 0;
    }
    rc = l3__egress_only_fec__create(unit,&l3eg_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rc;
    }

    *fec_id = l3eg_fec.fec_id;
    printf("FEC created. ID = 0x%x, for tunnel id = 0x%x\n", *fec_id, tunnel_id);

    return rc;
}


/* Create cascaded FEC */
int mpls_ecmp_cascaded_fec_create(int unit, int out_interface, int out_port, int out_vlan, bcm_mac_t next_hop_mac, bcm_if_t* fec_id) {
    int rc;

    create_l3_egress_s l3eg_fec;
    l3eg_fec.l3_flags = BCM_L3_CASCADED; /* Declare it's a second layer */
    l3eg_fec.out_tunnel_or_rif = out_interface;
    l3eg_fec.out_gport = out_port;
    l3eg_fec.vlan = out_vlan;
    l3eg_fec.fec_id = *fec_id;
    sal_memcpy(l3eg_fec.next_hop_mac_addr, next_hop_mac, 6);

    rc = l3__egress__create(unit,&l3eg_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rc;
    }
    *fec_id = l3eg_fec.fec_id;
    
    printf("Cascaded FEC created. ID = 0x%x, for tunnel id = 0x%x\n", *fec_id, out_interface);

    return rc;
}

/* Dual level ECMP with MPLS
   If slb_resolve is set- ecmp is resolved using Flexible-Hashing hw, otherwise using configured LB*/
int mpls_dual_ecmp_main(int unit, int in_port, int out_port, int slb_resolve) {
    int rc;
	uint32 reg32_val;

    /* Init default settings */
    mpls_ecmp_init();

    /* Create VLAN for incoming traffic */
    rc = vlan__open_vlan_per_mc(unit, mpls_ecmp_info_1.in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", mpls_ecmp_info_1.in_vlan, unit);
    }
    /* Assign in_port to vlan */
    rc = bcm_vlan_gport_add(unit, mpls_ecmp_info_1.in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, mpls_ecmp_info_1.in_vlan);
        return rc;
    }

    /* Create VLAN for outgoing traffic */
    rc = vlan__open_vlan_per_mc(unit, mpls_ecmp_info_1.out_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", mpls_ecmp_info_1.out_vlan, unit);
    }
    /* Assign out_port to vlan */
    rc = bcm_vlan_gport_add(unit, mpls_ecmp_info_1.out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, mpls_ecmp_info_1.out_vlan);
        return rc;
    }

    /* Creating in-rif to terminate ETH */
    create_l3_intf_s in_intf;
    in_intf.vsi = mpls_ecmp_info_1.in_vlan;
    in_intf.my_global_mac = mpls_ecmp_info_1.my_mac1;
    in_intf.my_lsb_mac = mpls_ecmp_info_1.my_mac1;
    in_intf.vrf = 1;
    in_intf.vrf_valid = 1;
    rc = l3__intf_rif__create(unit, &in_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* Creating out-lif */
    create_l3_intf_s out_intf;
    out_intf.vsi = mpls_ecmp_info_1.out_vlan;
    out_intf.my_global_mac = mpls_ecmp_info_1.my_mac2;
    out_intf.my_lsb_mac = mpls_ecmp_info_1.my_mac2;

    rc = l3__intf_rif__create(unit, &out_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* Creating two level-2 MPLS tunnels for outgoing packets */
    int tunnel_id_1, tunnel_id_2;
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label1, 0, &tunnel_id_1);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label2, 0, &tunnel_id_2);

    /* Creating six level-1 MPLS tunnels for outgoing packets */
    int tunnel_id_1_1, tunnel_id_1_2, tunnel_id_1_3, tunnel_id_2_1, tunnel_id_2_2, tunnel_id_2_3;

    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_1, out_intf.rif, &tunnel_id_1_1);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_2, out_intf.rif, &tunnel_id_1_2);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1_3, out_intf.rif, &tunnel_id_1_3);

    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_1, out_intf.rif, &tunnel_id_2_1);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_2, out_intf.rif, &tunnel_id_2_2);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2_3, out_intf.rif, &tunnel_id_2_3);

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

    if (is_device_or_above(unit, JERICHO2)) {
       ecmp1_1.flags |= BCM_L3_CASCADED;
    }

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

    if (is_device_or_above(unit, JERICHO2)) {
       ecmp1_2.flags |= BCM_L3_CASCADED;
    }

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1_2, interface_count, interfaces1_2);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-2 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x, 0x%x\n", ecmp1_2.ecmp_intf, interfaces1_2[0], interfaces1_2[1], interfaces1_2[2]);

    /* Creating two level-2 FECs for each level-2 MPLS tunnel and level-1 ECMPs */
    /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
       * In case we are using hierarchical FEC, the first FEC must be protected.
       * This was fixed in Jericho B0.
       */
    bcm_failover_t failover_id_fec;
    if (!is_device_or_above(unit,JERICHO_B0)) {
        rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_failover_create\n");
            return rc;
        }
        printf("failover_id = 0x%x was created for first level ECMP\n", failover_id_fec);
    }

    bcm_if_t fec_id1;
    bcm_if_t fec_id2;
    bcm_gport_t out_gport;

    /* level-1 MPLS tunnel will be chosen by ECMP1_1 or ECMP1_2*/
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_gport, ecmp1_1.ecmp_intf);
    rc = mpls_ecmp_first_fec_create(unit, tunnel_id_1, failover_id_fec, out_gport, &fec_id1);
    if (rc != BCM_E_NONE) {
        printf("Error, mpls_ecmp_first_fec_create\n");
        return rc;
    }


    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(out_gport, ecmp1_2.ecmp_intf);
    rc = mpls_ecmp_first_fec_create(unit, tunnel_id_2, failover_id_fec, out_gport, &fec_id2);
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
    l3host.l3a_vrf = in_intf.vrf;
    l3host.l3a_intf = ecmp1.ecmp_intf;

    
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host(route)_add failed: %x \n", rc);
        return rc;
    }

    return rc;
}

/*
 * This test create an hierarchical ECMP, the first level is of size 4 and all the FECs in the group points
 * to same next level ECMP also of size 4.
 * The first level ECMP add an MPLS tunnel with labels 0x1000...0x1003 and the second ECMP push another label
 * 0x2000...0x2003.
 */
int MPLS_ECMP_NOF_INTER = 4;
int mpls_hierarchal_ecmp_4by4(int unit,int in_port, int out_port) {

    uint8 my_mac1[6] = {0, 0, 0, 1, 1, 1};
    uint8 my_mac2[6] = {0, 0, 0, 1, 1, 2};
    uint8 next_hop_mac[6] = {0, 0, 0, 8, 8, 8};
    int in_vlan = 100;
    int out_vlan = 200;
    uint32 host = 0x37373737;/*55.55.55.55*/
    int rc = BCM_E_NONE,i;

    bcm_if_t fecLevel1[MPLS_ECMP_NOF_INTER],fecLevel2[MPLS_ECMP_NOF_INTER];

    /* Create VLAN for incoming traffic */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    /* Assign in_port to vlan */
    rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rc;
    }

    /* Create VLAN for outgoing traffic */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    /* Assign out_port to vlan */
    rc = bcm_vlan_gport_add(unit, in_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, in_vlan);
        return rc;
    }

    /* Creating in-rif to terminate ETH */
    create_l3_intf_s in_intf;
    in_intf.vsi = in_vlan;
    in_intf.my_global_mac = my_mac1;
    in_intf.my_lsb_mac = my_mac1;

    rc = l3__intf_rif__create(unit, &in_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* Creating out-lif */
    create_l3_intf_s out_intf;
    out_intf.vsi = out_vlan;
    out_intf.my_global_mac = my_mac2;
    out_intf.my_lsb_mac = my_mac2;

    rc = l3__intf_rif__create(unit, &out_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* create second level */
    int tunnel_id;
    for (i = 0;i < MPLS_ECMP_NOF_INTER; i++) {
        create_l3_egress_s l3eg_fec;
        mpls_ecmp_tunnel_initiator_create(unit, 2, 0x2000 + i, 0, &tunnel_id);

        l3eg_fec.l3_flags = BCM_L3_CASCADED; /* Declare it's a second layer */
        l3eg_fec.out_tunnel_or_rif = tunnel_id;
        l3eg_fec.out_gport = out_port;
        l3eg_fec.vlan = out_vlan;
        rc = l3__egress_only_fec__create(unit,&l3eg_fec);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }
        fecLevel2[i] = l3eg_fec.fec_id;
    }

    bcm_l3_egress_ecmp_t ecmp2;
    bcm_l3_egress_ecmp_t_init(&ecmp2);
    ecmp2.max_paths = MPLS_ECMP_NOF_INTER;
    if (is_device_or_above(unit, JERICHO2)) {
       ecmp2.flags |= BCM_L3_CASCADED;
    }

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp2, MPLS_ECMP_NOF_INTER, fecLevel2);
    if (rc != BCM_E_NONE) {
       printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
       return rc;
    }

    /*first level*/
    for (i = 0;i < MPLS_ECMP_NOF_INTER; i++) {
        mpls_ecmp_tunnel_initiator_create(unit, 1, 0x1000+i, out_intf.rif, &tunnel_id);
        create_l3_egress_s l3eg_fec;
        l3eg_fec.out_tunnel_or_rif = tunnel_id;
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3eg_fec.out_gport, ecmp2.ecmp_intf);
        sal_memcpy(l3eg_fec.next_hop_mac_addr, next_hop_mac, 6);

        if ((is_device_or_above(unit, JERICHO2) && 
               (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0))
            || !is_device_or_above(unit, JERICHO2)) {
            l3eg_fec.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
        } 

        l3eg_fec.fec_id = fecLevel1[i];
        if (!is_device_or_above(unit,JERICHO_B0)) {
            rc = l3__egress__create(unit,&l3eg_fec);
            if (rc != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rc;
            }
            l3eg_fec.failover_if_id = l3eg_fec.fec_id;
            l3eg_fec.arp_encap_id = 0;
        }
        rc = l3__egress__create(unit,&l3eg_fec);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }
        fecLevel1[i] = l3eg_fec.fec_id;
    }

    bcm_l3_egress_ecmp_t ecmp1;
    bcm_l3_egress_ecmp_t_init(&ecmp1);
    ecmp1.max_paths = MPLS_ECMP_NOF_INTER;

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1, MPLS_ECMP_NOF_INTER, fecLevel1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    /*Add route entry*/
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = host;
    l3rt.l3a_ip_mask = 0xFFFFFF00;
    l3rt.l3a_vrf = in_intf.vrf;
    l3rt.l3a_intf = ecmp1.ecmp_intf;
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %x \n", rc);
    }

    return rc;
}


