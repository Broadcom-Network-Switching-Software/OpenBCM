/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Sequence example to
 * test:
 * run:
 * BCM> cint ../sand/utility/cint_sand_utils_mpls.c
 * BCM> cint utility/cint_utils_l3.c
 * BCM> cint ../sand/utility/cint_sand_utils_l3.c
 * BCM> cint cint_mpls_lsr.c
 * BCM> cint cint_ip_route.c
 * BMC> cint cint_mutli_device_utils.c
 * BCM> cint cint_ip_route_tunnel.c
 * BCM> cint cint_qos_l3_remark.c
 * BCM> cint
 * cint> int nof_units = <nof_units>;
 * cint> int units[nof_units] = {<unit1>, <unit2>,...};
 * cint> int outP = 13;
 * cint> int inP = 13;
 * cint> int outSysport, inSysport;
 * cint> port_to_system_port(unit1, outP, &outSysport);
 * cint> port_to_system_port(unit2, inP, &inSysport);
 * example_ip_to_tunnel(units_ids, nof_units,<inSysport>, <outSysport>);
 *
 *
 * Note: this cint also includes tests for 4 label push and protection
 */

/* *********
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 1;


/*
 * When using MPLS tunnel, whether to add EL/EL+ELI
 * 0 - No entropy
 * 1 - EL
 * 2 - EL+ELI
 */
int add_el_eli = 0;

int jre2_legal_fec_id = 0;

/**********
  functions
 */

/******* Run example ******/

/*
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
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - vlan = 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
 */
int preserve_dscp_per_lif = 0;
int mpls_tunnel_used = 0;
int example_ip_to_tunnel(int ingress_unit, int egress_unit, int in_sysport, int out_sysport){
    int rv;
    int unit, i;
    int ing_intf_in;
    int ing_intf_out;
    int fec[2] = {0x0,0x0};
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0};
    int open_vlan=1;
    int route;
    int mask;
    int tunnel_id, tunnel_id2 = 0;
	int flags = 0;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties2;

    create_l3_intf_s intf;

    int sysport_is_local = 0;
    int local_port = 0;

    /*** create ingress router interface ***/
    ing_intf_in = 0;
    unit = ingress_unit;
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }

    if (preserve_dscp_per_lif & 0x1) {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port=in_sysport;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = in_vlan;
        vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : in_vlan;
        vlan_port.vsi = in_vlan;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("fail create VLAN port, port(0x%08x), vlan(%d)\n", in_sysport, in_vlan);
            return rv;
        }

        rv = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
            return rv;
        }
    } else {
        sysport_is_local = 0;
        rv = system_port_is_local(unit, in_sysport, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sysport_is_local().\n");
            return rv;
        }
        if(sysport_is_local == 1) {
            rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
                return rv;
            }
        }
    }

    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    unit = egress_unit;
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }

    sysport_is_local = 0;
    rv = system_port_is_local(unit, out_sysport, &local_port, &sysport_is_local);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sysport_is_local().\n");
        return rv;
    }
    if(sysport_is_local == 1) {
        rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
            return rv;
        }
    }

    intf.vsi = out_vlan;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*Tunnel 1*/

    unit = egress_unit;    
    create_l3_egress_s l3eg;

    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.vlan = out_vlan;

    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }
    encap_id[0] = l3eg.arp_encap_id;
    ing_intf_out = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
    }

    /*** create egress object 1 ***/
    /*** Create tunnel ***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    tunnel_id = 0;
    unit = egress_unit;
    
    mpls_tunnel_properties.label_in = 200;
    mpls_tunnel_properties.label_out = 0;
    mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel2;
    mpls_tunnel_properties.next_pointer_intf = ing_intf_out;

    if (add_el_eli >= 2) {
        mpls_tunnel_properties.flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
        if (add_el_eli == 3) {
            mpls_tunnel_properties.encap_access = bcmEncapAccessTunnel1;
            mpls_tunnel_properties.label_out = 0x300;
            mpls_tunnel_properties.label_out_2 = 0x320;
            mpls_tunnel_properties.label_out_3 = 0x330;
            mpls_tunnel_properties.flags_out |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            mpls_tunnel_properties.flags_out_2 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            mpls_tunnel_properties.flags_out_3 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
        }
    }
    else if (add_el_eli == 1) {
        mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }
    printf("Trying to create tunnel initiator\n");
	rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
	if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }
    /* having a tunnel id != 0 is equivalent to being WITH_ID*/

    tunnel_id = mpls_tunnel_used = mpls_tunnel_properties.tunnel_id;
    printf("Created Tunnel ID: 0x%08x\n",tunnel_id);

    /*Tunnel 2*/

    /*** Create ARP2 first***/
    create_l3_egress_s l3eg1;

    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
    l3eg1.vlan = out_vlan;

    rv = l3__egress_only_encap__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }
    encap_id[1] = l3eg1.arp_encap_id;
    ing_intf_out = l3eg1.arp_encap_id;
    if(verbose >= 1) {
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
    }

    mpls_tunnel_properties2.label_in = 200;
    mpls_tunnel_properties2.label_out = 0;
    mpls_tunnel_properties2.next_pointer_intf = ing_intf_out;

    if (add_el_eli >= 2) {
        mpls_tunnel_properties2.flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
        if (add_el_eli == 3) {
            mpls_tunnel_properties2.label_out = 0x300;
            mpls_tunnel_properties2.label_out_2 = 0x320;
            mpls_tunnel_properties2.label_out_3 = 0x330;
            mpls_tunnel_properties2.flags_out |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            mpls_tunnel_properties2.flags_out_2 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            mpls_tunnel_properties2.flags_out_3 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                         BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
        }
    }
    else if (add_el_eli == 1) {
        mpls_tunnel_properties2.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }
    printf("Trying to create tunnel initiator\n");
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties2);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }
    /* having a tunnel id != 0 is equivalent to being WITH_ID*/

    tunnel_id2 = mpls_tunnel_properties2.tunnel_id;
    printf("Created Tunnel ID: 0x%08x\n",tunnel_id2);

    if (preserve_dscp_per_lif & 0x2) {
        bcm_gport_t gport;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, tunnel_id);
        rv = bcm_port_control_set(unit, gport, bcmPortControlPreserveDscpEgress, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", gport);
            return rv;
        }
    }

    /*Ingress fwd for tunnel 1*/
    flags = 0;
    unit = ingress_unit;
    /*** Create egress object1 with the tunnel_id ***/

    sal_memset(l3eg1.next_hop_mac_addr, 0 , 6);
    l3eg1.vlan = 0;
    l3eg.out_tunnel_or_rif = tunnel_id;
    l3eg.arp_encap_id = 0;
    l3eg.out_gport = out_sysport;
    rv = l3__egress_only_fec__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    fec[0] = l3eg.fec_id;
    encap_id[0] = l3eg.out_tunnel_or_rif;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    if (!is_device_or_above(unit, JERICHO2)){
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]);
    }else {
        route = 0x7fffff03;
        mask = 0xffffffff;
        rv = internal_ip_route(unit, route, mask, vrf, fec[0], 0) ;
    }
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
    }
    
    flags = 0;
    unit = ingress_unit;

    /*** create egress object 2***/
    create_l3_egress_s l3eg2;

    l3eg2.out_tunnel_or_rif = tunnel_id2;
    l3eg2.out_gport = out_sysport;

    rv = l3__egress_only_fec__create(unit,&l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    fec[1] = l3eg2.fec_id;
    encap_id[1] = l3eg2.out_tunnel_or_rif;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    unit = ingress_unit;

    rv = add_route(unit, route, mask , vrf, fec[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);
    }


    return rv;
}


/*
     custom_feature_preserving_dscp_enabled=1
     custom_feature_dscp_preserve_with_extend_label=1
     mpls_egress_label_extended_encapsulation_mode=1
     logical_port_routing_preserve_dscp=2

     cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
     cint ../../../../src/examples/dpp/cint_qos.c
     cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
     cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
     cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
     cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
     cint ../../../../src/examples/dpp/cint_mpls_lsr.c
     cint ../../../../src/examples/dpp/cint_ip_route.c
     cint ../../../../src/examples/dpp/cint_multi_device_utils.c
     cint ../../../../src/examples/dpp/cint_ip_route_tunnel.c
     cint ../../../../src/examples/dpp/internal/sqa/cint_mpls_l3vpn_comb.c
     cint
     preserve_dscp_per_lif =2;
     int nof_units = 1;
     int units[1] = {0};
     int outP = 200;
     int inP = 201;
     int outSysport, inSysport;
     print port_to_system_port(units[0], outP, &outSysport);
     print port_to_system_port(units[0], inP, &inSysport);
     print example_ip_to_tunnel_ext_label(units, nof_units, inSysport, outSysport, 4);
     exit;
     debug bcm rx
     tx 1 psrc=201 data=000c000200000000070001008100000f0800458c003d000000008011b11c0a0000057fffff03c351000000290000000102030405060708090a0b0c0d0e0f1011

     tx 1 psrc=201 data=000C000200000000070000018100000F86DD60300000002A3BFFFE80000000000000020007FFFE00000120010000000000000000000000000004000102030405
 */



int create_C_tunnel(int unit, int encap_mode, int next_pointer)
{
    int rv;

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    uint64 wide_data;

    mpls_tunnel_properties.force_flags=1;
    mpls_tunnel_properties.flags=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    mpls_tunnel_properties.label_in = additional_labels_info.C1_label;
    mpls_tunnel_properties.label_out = encap_mode ? additional_labels_info.C2_label : 0;
    mpls_tunnel_properties.ttl = additional_labels_info.C1_ttl;
    mpls_tunnel_properties.ext_ttl = encap_mode ? additional_labels_info.C2_ttl : 0;
    mpls_tunnel_properties.exp = additional_labels_info.C1_exp;
    mpls_tunnel_properties.ext_exp = encap_mode ? additional_labels_info.C2_exp : 0;
    mpls_tunnel_properties.with_exp = 1;
    mpls_tunnel_properties.next_pointer_intf = next_pointer;

    mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_WIDE;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    additional_labels_info.C_intf = mpls_tunnel_properties.tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.C_gport, mpls_tunnel_properties.tunnel_id);

    /*set for C3*/
    if ((encap_mode == 2) || (encap_mode == 3) || (encap_mode == 4)) {
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
    }

    printf("Create C tunnel with encap_mode[%d]\n", encap_mode);
    return rv;
}

int create_B_tunnel(int unit, int encap_mode)
{
    int rv;

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    sal_memset(&mpls_tunnel_properties, 0, sizeof(mpls_tunnel_properties));
    mpls_tunnel_properties.force_flags=1;
    mpls_tunnel_properties.flags=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    mpls_tunnel_properties.label_in = (encap_mode >= 3) ? additional_labels_info.B1_label : 0;
    mpls_tunnel_properties.label_out = (encap_mode & 4) ? additional_labels_info.B2_label : 0;
    mpls_tunnel_properties.ttl = (encap_mode >= 3) ? additional_labels_info.B1_ttl : 0;
    mpls_tunnel_properties.ext_ttl = (encap_mode & 4) ? additional_labels_info.B2_ttl : 0;
    mpls_tunnel_properties.exp = (encap_mode >= 3) ? additional_labels_info.B1_exp : 0;
    mpls_tunnel_properties.ext_exp = (encap_mode & 4) ? additional_labels_info.B2_exp : 0;
    mpls_tunnel_properties.with_exp = 1;
    mpls_tunnel_properties.next_pointer_intf = additional_labels_info.C_intf;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }
    additional_labels_info.B_intf = mpls_tunnel_properties.tunnel_id;

    printf("Create B tunnel with encap_mode[%d]\n", encap_mode);
    return rv;
}

/*
 * brief:
 *  create MPLS tunnels according to the encap mode.
 * encap_mode[in]:
 *  0: C1
 *  1: C1+C2
 *  2: C1+C2+C3
 *  3: B1+C1+C2+C3
 *  4: B1+B2+C1+C2+C3
 *  5: B1+C1+C2
 *  6: B1+B2+C1+C2
 * next_pointer[in]:
 *  next encapsulation pointer following MPLS tunnel. Generally, it may be ARP.
 * last_tunnel_id[out]:
 *  last(outmost) mpls tunnel encap entry pointer. It is a must for JR2, since JR2
 *  won't find the last tunnel automatically. For JR, it is the same as the inner-.
 *  most tunnel encap pointer which was returned in additional_labels_info.B_intf/C_intf.
 */
int tunnels_create(int unit, int encap_mode, int next_pointer, int * last_tunnel_id)
{
    int rv;

    if (!is_device_or_above(unit, JERICHO2)) {
        rv = create_C_tunnel(unit, encap_mode, next_pointer);
        if (rv != BCM_E_NONE) {
            printf("Error, in create_C_tunnel\n");
            return rv;
        }

        if (encap_mode >= 3) {
            rv = create_B_tunnel(unit, encap_mode);
            if (rv != BCM_E_NONE) {
                printf("Error, in create_B_tunnel\n");
                return rv;
            }
        }

        *last_tunnel_id = (encap_mode > 2) ? additional_labels_info.B_intf : additional_labels_info.C_intf;
    } else {
        /** We achieve the same mpls tunnel encap as in JR per encap_mode below.*/
        bcm_mpls_egress_label_t label_array[5];
        int label_index;
        int nof_labels;

        /** Initialize the label_array first.*/
        for (label_index = 0; label_index < 5; label_index++) {
            bcm_mpls_egress_label_t_init(&(label_array[label_index]));

            label_array[label_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            /** Only PHP action can be input, other actions are resolved by hw in JR2.*/
            label_array[label_index].action = 0;
        }

        label_array[0].label = additional_labels_info.B1_label;
        label_array[1].label = additional_labels_info.B2_label;
        label_array[2].label = additional_labels_info.C1_label;
        label_array[3].label = additional_labels_info.C2_label;
        label_array[4].label = additional_labels_info.C3_label;

        /** no need to set exp and ttl for uniform mode*/
        /*
        label_array[0].exp = additional_labels_info.B1_exp;
        label_array[1].exp = additional_labels_info.B2_exp;
        label_array[2].exp = additional_labels_info.C1_exp;
        label_array[3].exp = additional_labels_info.C2_exp;
        label_array[4].exp = 7;

        label_array[0].ttl = additional_labels_info.B1_ttl;
        label_array[1].ttl = additional_labels_info.B2_ttl;
        label_array[2].ttl = additional_labels_info.C1_ttl;
        label_array[3].ttl = additional_labels_info.C2_ttl;
        label_array[4].ttl = 255;
        */
 
        /** Adjust the lable_array by label_index and nof_labels per the encap_mode.*/
        if (encap_mode <= 2) {
            /** Create C tunnel only*/
            label_index = 2;
            nof_labels = encap_mode + 1;
        } else if (encap_mode == 4 || encap_mode == 6) {
            /** Create B and C tunnel*/
            label_index = 0;
            nof_labels = (encap_mode == 6) ? 4 : 5;
        } else if (encap_mode == 3 || encap_mode == 5) {
            /** Create B and C tunnel*/
            label_index = 1;
            nof_labels = (encap_mode == 5) ? 3 : 4;

            label_array[1].label = label_array[0].label;
            label_array[1].exp = label_array[0].exp;
            label_array[1].ttl = label_array[0].ttl;
        } else {
            return BCM_E_PARAM;
        }

        /** Create the tuunels. Note: label_index is indicating the first tunnel in the array.*/
        rv = sand_mpls_tunnel_initiator_create(unit, next_pointer, nof_labels, &(label_array[label_index]));
        if (rv != BCM_E_NONE) {
            printf("Error, in sand_mpls_tunnel_initiator_create\n");
            return rv;
        }

        /** Encoding the tunnel index(inner-most tunnel index is returned) for output.*/
        additional_labels_info.C_intf = label_array[label_index].tunnel_id;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.C_gport, additional_labels_info.C_intf);

        if (encap_mode > 2) {
            additional_labels_info.B_intf = additional_labels_info.C_intf;
        }

        /** Return the last(outmost) tunnel index for JR2 specially.*/
        *last_tunnel_id = label_array[label_index + nof_labels - 1].tunnel_id;
    }

    return rv;
}

int example_ip_to_tunnel_ext_label(int *units_ids, int nof_units, int in_sysport, int out_sysport, int encap_mode){
    int rv;
    int unit, i;
    int ing_intf_in;
    int ing_intf_out;
    int fec[2] = {0x0,0x0};
    int in_vlan = 15;
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0};
    int open_vlan=1;
    int route;
    int mask;
    int tunnel_id, tunnel_id2 = 0;
    int last_tunnel_id = 0;
    int flags = 0;
    int ing_qos_profile = 7, egr_qos_profile = 2;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties2;

    create_l3_intf_s intf;
    uint64 wide_data;

    additional_labels_info.B1_label = 0x11;
    additional_labels_info.B2_label = 0x22;
    additional_labels_info.C1_label = 0x33;
    additional_labels_info.C2_label = 0x44;
    additional_labels_info.C3_label = 0x55;
    additional_labels_info.D1_label = 0x66;

    additional_labels_info.B1_ttl = 16;
    additional_labels_info.B2_ttl = 16;
    additional_labels_info.C1_ttl = 16;
    additional_labels_info.C2_ttl = 64;
    additional_labels_info.B1_exp = 3;
    additional_labels_info.B2_exp = 3;
    additional_labels_info.C1_exp = 3;
    additional_labels_info.C2_exp = 1;

    /*** create ingress router interface ***/
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        if (preserve_dscp_per_lif & 0x1) {
            bcm_vlan_port_t vlan_port;
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.port=in_sysport;
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan = in_vlan;
            vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : in_vlan;
            vlan_port.vsi = in_vlan;
            rv = bcm_vlan_port_create(unit, &vlan_port);
            if (rv != BCM_E_NONE) {
                printf("fail create VLAN port, port(0x%08x), vlan(%d)\n", in_sysport, in_vlan);
                return rv;
            }

            rv = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
                return rv;
            }
        } else {
            rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
              return rv;
            }
        }

        intf.vsi = in_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.qos_map_valid = (ing_qos_profile != -1);
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_id_get_by_profile(unit, flags, ing_qos_profile, &intf.qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_qos_map_id_get_by_profile\n", rv);
        }

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        }
        rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
          return rv;
        }

        intf.vsi = out_vlan;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_out = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress object 1 ***/
    /*** Create tunnel ***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    tunnel_id = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = tunnels_create(unit, encap_mode, 0, &last_tunnel_id);
        if (rv != BCM_E_NONE) {
            printf("Error %d, tunnels_create\n", rv);
            return rv;
        }

        /*
         * Set egress qos profile to tunnel-gport.
         * In JR2, dscp is remarked with {egr_qos_profile, fthm.nwk_qos}.qos.
         * In JR, dscp is remarked with {0, fthm.nwk_qos}.qos since bcm_qos_port_map_set can't be used for mpls tunnel gport.
         */
        if (is_device_or_above(unit, JERICHO2)) {
            int qos_map_id;
            int route_qos_lif;
            flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
            rv = bcm_qos_map_id_get_by_profile(unit, flags, egr_qos_profile, &qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error %d, bcm_qos_map_id_get_by_profile\n", rv);
            }

            route_qos_lif = (encap_mode > 2) ? additional_labels_info.B_intf : additional_labels_info.C_intf;
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(route_qos_lif, route_qos_lif);
            rv = bcm_qos_port_map_set(unit, route_qos_lif, -1, qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error %d, bcm_qos_port_map_set\n", rv);
            }
        }
    }

    /*** Create egress object1 with the tunnel_id ***/
    flags = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        sand_utils_l3_arp_s arp_structure;
        sand_utils_l3_arp_s_init(unit, 0, &arp_structure);
        sal_memcpy(&(arp_structure.next_hop_mac_addr), next_hop_mac , 6);
        arp_structure.allocation_flags = flags;
        arp_structure.intf_id = last_tunnel_id;
        arp_structure.arp_id = encap_id[0];
        arp_structure.eth_rif = out_vlan;

        rv = sand_utils_l3_arp_create(unit, &arp_structure);
        if (rv != BCM_E_NONE) {
            printf("Error, sand_utils_l3_arp_create\n");
            return rv;
        }
        encap_id[0] = arp_structure.l3eg_arp_id;

        sand_utils_l3_fec_s fec_structure;
        sand_utils_l3_fec_s_init(unit, 0, &fec_structure);
        fec_structure.fec_id = fec[0];
        fec_structure.destination = out_sysport;
        fec_structure.tunnel_gport = (encap_mode > 2) ? additional_labels_info.B_intf : additional_labels_info.C_intf;
        fec_structure.tunnel_gport2 = 0;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE) {
            printf("Error, sand_utils_l3_fec_create\n");
            return rv;
        }
        fec[0] = fec_structure.l3eg_fec_id;

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    flags = 0;
    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        l3_egress_s l3eg1;
        sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg1.allocation_flags = flags;
        l3eg1.out_tunnel_or_rif = tunnel_id2;
        l3eg1.vlan = out_vlan;
        l3eg1.out_gport = out_sysport;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];
        rv = sand_utils_l3_egress_create(unit, &l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, sand_utils_l3_egress_create\n");
            return rv;
        }
        encap_id[1] = l3eg1.arp_encap_id;
        fec[1] = l3eg1.fec_id;

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** add routing info to FEC ***/
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /*** add host point to FEC ***/
        host = 0x7fffff03;
        mask = 0xffffffff;
        if (is_device_or_above(unit, JERICHO2) && vrf == 0) {
            /**In JR2, public routes should always reside in route table.*/
            rv = add_route(unit, host, mask , vrf, fec[0]);
            if (rv != BCM_E_NONE) {
                printf("Error %d, add_route\n", rv);
            }
        } else {
            rv = add_host(unit, 0x7fffff03, vrf, fec[0]);
            if (rv != BCM_E_NONE) {
                printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
            }
        }

        /*** add routes point to FEC ***/
        route = 0x7fffff00;
        mask  = 0xfffffff0;
        rv = add_route(unit, route, mask , vrf, fec[1]);
        if (rv != BCM_E_NONE) {
            printf("Error %d, add_route\n", rv);
        }

        bcm_ip6_t ip6_mask;
        if (is_device_or_above(unit, JERICHO2)) {
            /** In JR2, the mask sequence is Reverted*/
            int i;
            for (i = 0; i < 16; i++) {
                ip6_mask[i] = mask_l[15-i];
            }
        } else {
            sal_memcpy(&(ip6_mask), (mask_l), 16);
        }
        rv = add_route_ip6(unit, host_r, ip6_mask, 0, fec[0]);
        if (rv != BCM_E_NONE) {
            printf("Error %d, add_route_ip6\n", rv);
        }
    }

    return rv;
}

/*
 * outlif.remark_profile is used to set qos preserve by BCM_QOS_OPCODE_PRESERVE.
 * Note: only routing case is supported.
 */
int example_ip_to_tunnel_qos_preserve(int unit, int encap_mode, int qos_profile, int enable)
{
    int rv =BCM_E_NONE;
    int qos_map_id;
    int dscp_preserve_enable = enable ? 1 : 0;
    uint32 flags;

    if ((preserve_dscp_per_lif & 0x2) && !is_device_or_above(unit, JERICHO2)) {
        int route_qos_lif;
        route_qos_lif = (encap_mode > 2) ? additional_labels_info.B_intf : additional_labels_info.C_intf;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(route_qos_lif, route_qos_lif);
        rv = bcm_port_control_set(unit, route_qos_lif, bcmPortControlPreserveDscpEgress, dscp_preserve_enable);
        if (rv != BCM_E_NONE) {
            printf("set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", route_qos_lif);
            return rv;
        }
        printf("set bcmPortControlPreserveDscpEgress successfully for port(0x08x) with value of %d\n", route_qos_lif, dscp_preserve_enable);
    } else if (is_device_or_above(unit, JERICHO2)) {
        rv = qos_l3_dscp_preserve_per_outlif_set(unit, qos_profile, dscp_preserve_enable);
        if (rv != BCM_E_NONE) {
            printf("Error %d, qos_l3_dscp_preserve_per_outlif_set for dscp preserve.\n", rv);
        }
    }

    return rv;
}

