/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* 
 * This cint is used as an example of IP host configuration with encapsulation of the following stack: 
 *  
 *                   |--------------|
 *                   |     PDU      |
 *                   |--------------|
 *                   |  Native IP   |
 *                   |--------------|
 *                   |     EL       |
 *                   |--------------|
 *                   |     ELI      |
 *                   |--------------|   
 *                   |     B1       |
 *                   |--------------| 
 *                   |     B2       |
 *                   |--------------|
 *                   |     C1       |
 *                   |--------------|   
 *                   |     C2       |
 *                   |--------------|  
 *                   |     C3       |
 *                   |--------------|  
 *                   |     D1       |
 *                   |--------------|
 *                   |     LL       |
 *                   |--------------| 
 *  
 *  
 * Sequence example:  
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../src/examples/dpp/cint_qos.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/cint_ip_route.c
 * cint ../../src/examples/dpp/cint_ip_route_tunnel.c
 * cint ../../src/examples/dpp/cint_ip_route_tunnel_mpls_additional_labels.c
 * cint
 * example_ip_route_to_tunnel_mpls_additional_labels(0,1811939531,1811939528,1,1,1,0,0);
 * exit;
 *
 * tx 1 psrc=203 data=0x000c000200000000070001008100000f080045000035000000008000b1c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x00000000cd1d000c0002000081000064884700044240000334200002261000011808000078080000110045000035000000007f00b2c10a0000057fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a b1c1d1e1f20 
 *  
 *
 * packet will be routed from in_port to out-port 
 * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.03)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  MPLS labels:
 * Different labels can be chosen to be encapsulated according to cint parameters: 
 * add_EL_ELI
 * add_B1_B2 
 * add_C1_C2 
 * add_C3
 * add_D1 
 */

/* ********* 
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 1;

struct ip_route_to_tunnel_additional_labels_s {

    int B1_label;
    int B2_label;
    int C1_label;
    int C2_label;
    int C3_label;
    int D1_label;

    int B1_ttl;
    int B2_ttl;
    int C1_ttl;
    int C2_ttl;
    int B1_exp;
    int B2_exp;
    int C1_exp;
    int C2_exp;

    int C_intf;
    int C_gport;
    int B_intf;
    int B_gport;
    int D_gport;

    int in_vlan; 
    int out_vlan;
    int host;
    int vrf;
    bcm_mac_t mac_address[6];  /* my-MAC */
    bcm_mac_t next_hop_mac[6]; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2[6]; /* next_hop_mac1 */
};

ip_route_to_tunnel_additional_labels_s additional_labels_info;

/********** 
  functions
 */

void
ip_route_to_tunnel_additional_labels_init(int unit)
{
    uint8 mac_address[6]  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    uint8 next_hop_mac[6]  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    uint8 next_hop_mac2[6]  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    
    additional_labels_info.B1_label = 0x11;
    additional_labels_info.B2_label = 0x22;
    additional_labels_info.C1_label = 0x33;
    additional_labels_info.C2_label = 0x44;
    additional_labels_info.C3_label = 0x55;
    additional_labels_info.D1_label = 0x66;

    additional_labels_info.B1_ttl = 8;
    additional_labels_info.B2_ttl = 16;
    additional_labels_info.C1_ttl = 32;
    additional_labels_info.C2_ttl = 64;
    additional_labels_info.B1_exp = 4;
    additional_labels_info.B2_exp = 3;
    additional_labels_info.C1_exp = 2;
    additional_labels_info.C2_exp = 1;

    additional_labels_info.in_vlan = 15; 
    additional_labels_info.out_vlan = 100;
    additional_labels_info.host = 0x7fffff03;
    additional_labels_info.vrf = 0;
    sal_memcpy(additional_labels_info.mac_address, mac_address, 6);
    sal_memcpy(additional_labels_info.next_hop_mac, next_hop_mac, 6);
    sal_memcpy(additional_labels_info.next_hop_mac2, next_hop_mac2, 6);

}

int
cint_ip_route_to_tunnel_additional_labels__create_rif(int unit, int vlan, int sys_port, int * intf)
{
    int rv;
    create_l3_intf_s l3_intf_info;

    rv = vlan__open_vlan_per_mc(unit, vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, vlan, sys_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", sys_port, vlan);
        return rv;
    }

    l3_intf_info.vsi = vlan;
    sal_memcpy(l3_intf_info.my_global_mac, additional_labels_info.mac_address, 6);
    sal_memcpy(l3_intf_info.my_lsb_mac, additional_labels_info.mac_address, 6);
    l3_intf_info.vrf_valid = 1;
    l3_intf_info.vrf = additional_labels_info.vrf;

    rv = l3__intf_rif__create(unit, &l3_intf_info);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    *intf = l3_intf_info.rif;

    return rv;
}

int
cint_ip_route_to_tunnel_additional_labels__add_ll_label(int unit, int gport, int label) {
    int rv;
    uint64 wide_data;
    int get_value;
    int intf;

    /*
     * This is a TEMPORARY solution, to make sure this procedure detects input 'gport' as
     * interface (and, consequently, encodes it as 'gport tunnel'
     * This issue is also found in two other files.
     * The final solution should be to inform this procedure on whether input 'gport' is
     * actually encoded as 'gport' or as 'intrface'.
     */
    if (!(BCM_GPORT_IS_TUNNEL(gport))) {
        if (BCM_L3_ITF_TYPE_IS_LIF(gport)) {
            intf = gport;
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);
        }
    }

    /* Fill wide entry and configure outlif as having additional label */
    COMPILER_64_SET(wide_data, 0, label);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_wide_data_set\n");
        return rv;
    }

    rv = bcm_port_control_set(unit, gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    rv = bcm_port_control_get(unit, gport, bcmPortControlMPLSEncapsulateAdditionalLabel, &get_value);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_get\n");
        return rv;
    }
    if (get_value != 1) {
        printf("Error, in bcm_port_control_get: value is not 1\n");
        return BCM_E_INTERNAL;
    }

    return rv;
}

/******* Run example ******/
int example_ip_route_to_tunnel_mpls_additional_labels(int unit, int in_sysport, int out_sysport, int add_EL_ELI, int add_B1_B2, int add_C1_C2, int add_C3, int add_D1, int add_B3) {
    int rv;
    int ing_intf_in_dummy, ing_intf_out;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    create_l3_intf_s intf;
    create_l3_egress_s l3_egress;

    /* validity */
    if (!add_B1_B2 && !add_C1_C2) {
        printf("Error, at least C or B tunnels must exist \n");
        return BCM_E_PARAM;
    }

    if (add_C3 && !add_C1_C2) {
        printf("Error, C3 canot exist without C1 and C2 tunnel \n");
        return BCM_E_PARAM;
    }

    /* Init params */
    ip_route_to_tunnel_additional_labels_init(unit);

    mpls_pipe_mode_exp_set = 1;
    rv = mpls__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__init\n");
    }

    /*** create ingress router interface ***/
    rv = cint_ip_route_to_tunnel_additional_labels__create_rif(unit, additional_labels_info.in_vlan, in_sysport, &ing_intf_in_dummy);
    if (rv != BCM_E_NONE) {
        printf("Error, in cint_ip_route_to_tunnel_additional_labels__create_rif\n");
    }

    /*** create egress router interface ***/
    rv = cint_ip_route_to_tunnel_additional_labels__create_rif(unit, additional_labels_info.out_vlan, out_sysport, &ing_intf_out);
    if (rv != BCM_E_NONE) {
        printf("Error, in cint_ip_route_to_tunnel_additional_labels__create_rif\n");
    }

    /* 
     *  IPoELoELIoB1oB2oC1oC2oC3oD1oLL 
     *  Create MPLS stack:
     *  First create C1 & C2 if exist
     *  C3 is extention of C labels
     */
    if (add_C1_C2) {
        mpls_tunnel_properties.label_in = additional_labels_info.C1_label;
        mpls_tunnel_properties.label_out = additional_labels_info.C2_label;
        mpls_tunnel_properties.ttl = additional_labels_info.C1_ttl;
        mpls_tunnel_properties.ext_ttl = additional_labels_info.C2_ttl;
        mpls_tunnel_properties.exp = additional_labels_info.C1_exp;
        mpls_tunnel_properties.ext_exp = additional_labels_info.C2_exp;
        mpls_tunnel_properties.with_exp = 1;
        mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
        if (add_C3) {
            mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_WIDE;
        }
        printf("Trying to create tunnel initiator\n");
        rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls__create_tunnel_initiator__set\n");
            return rv;
        }

        additional_labels_info.C_intf = mpls_tunnel_properties.tunnel_id;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.C_gport, mpls_tunnel_properties.tunnel_id);
        printf("Created Tunnel ID: 0x%08x\n", additional_labels_info.C_intf);

        if (add_C3) {
            uint64 wide_data;

            COMPILER_64_SET(wide_data, 0, additional_labels_info.C3_label);

            rv = bcm_port_wide_data_set(unit, additional_labels_info.C_gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_wide_data_set\n");
                return rv;
            }

            rv = bcm_port_control_set(unit, additional_labels_info.C_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_control_set\n");
                return rv;
            }

            int get_value;
            rv = bcm_port_control_get(unit, additional_labels_info.C_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, &get_value);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_control_get\n");
                return rv;
            }
            if (get_value != 1) {
                printf("Error, in bcm_port_control_get: value is not 1\n");
                return BCM_E_INTERNAL;
            }
        }
    }

    /* 
     *  IPoELoELIoB1oB2oC1oC2oC3oD1oLL
     *  Set B1&B2 if exist
     *  El & ELI are properties of B labels
     */

    if (add_B1_B2) {
        sal_memset(&mpls_tunnel_properties, 0, sizeof(mpls_tunnel_properties));

        mpls_tunnel_properties.label_in = additional_labels_info.B1_label;
        mpls_tunnel_properties.label_out = additional_labels_info.B2_label;
        mpls_tunnel_properties.ttl = additional_labels_info.B1_ttl;
        mpls_tunnel_properties.ext_ttl = additional_labels_info.B2_ttl;
        mpls_tunnel_properties.exp = additional_labels_info.B1_exp;
        mpls_tunnel_properties.ext_exp = additional_labels_info.B2_exp;
        mpls_tunnel_properties.with_exp = 1;
        mpls_tunnel_properties.next_pointer_intf = add_C1_C2 ? additional_labels_info.C_intf : ing_intf_out;
        if (add_EL_ELI) {
            mpls_tunnel_properties.flags_out = mpls_tunnel_properties.flags;
            mpls_tunnel_properties.flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE |
                                                BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
        }
        if (add_B3) {
            mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_WIDE;
        }
        printf("Trying to create tunnel initiator\n");
        rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls__create_tunnel_initiator__set\n");
            return rv;
        }

        additional_labels_info.B_intf = mpls_tunnel_properties.tunnel_id;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.B_gport, mpls_tunnel_properties.tunnel_id);
        printf("Created Tunnel ID: 0x%08x\n", additional_labels_info.B_intf);
    }

    /* 
     *  IPoELoELIoB1oB2oC1oC2oC3oD1oLL
     *  Set LL + FEC
     *  D1 is property of LL
     */

    /* ARP */
    l3_egress.vlan = additional_labels_info.out_vlan;
    sal_memcpy(l3_egress.next_hop_mac_addr, additional_labels_info.next_hop_mac, 6);

    /* FEC */
    l3_egress.out_tunnel_or_rif = add_B1_B2 ? additional_labels_info.B_intf : additional_labels_info.C_intf /* at least B or C must exist */;
    l3_egress.out_gport = out_sysport;

    l3_egress.l3_flags2 = add_D1 ? BCM_L3_FLAGS2_EGRESS_WIDE : 0;

    rv = l3__egress__create(unit, &l3_egress); 
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress__create\n");
    }

    printf("l3_egress.encap_id: %d\n", l3_egress.arp_encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.D_gport, l3_egress.arp_encap_id);
        

    if (add_D1) {
        rv = cint_ip_route_to_tunnel_additional_labels__add_ll_label(unit, additional_labels_info.D_gport, additional_labels_info.D1_label);
        if (rv != BCM_E_NONE) {
        rv = bcm_port_control_get(unit, additional_labels_info.D_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, &get_value);
            printf("Error, in bcm_port_control_get: value is not 1\n");
        }
    }

    /*** add host point to FEC ***/
    rv = add_host(unit, additional_labels_info.host, additional_labels_info.vrf, l3_egress.fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
    }

    return rv;
}


