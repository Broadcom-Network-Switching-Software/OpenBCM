/*~~~~~~~~~~~~~~~~~~~~~~~MPLS NOP ENCAPSULATION~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_nop_action_encapsulation.c
 * Purpose: Example of encapsulating NOP entry.
 *
 * This cint is an example of MPLS NOP entry encapsulation.
 *
 * This configuration includes:
 *      - Set up a MPLS LSR path with swap performed on egress (ILM action is NOP, Egress swap is done by mpls_tunnel_initiator)
 *              The result of the ILM lookup will be a FEC destination
 *              The FEC will resolve to 
 *	                Out-LIF = MPLS SWAP EEDB entry
 *	                TM destination (port).
 *	    - Use the ingress PMF to modify the forwarding information to test the NOP EEDB entry:
 *	            update the Out-LIF; Out-LIF = pointer to EEDB NOP EEDB entry 
 *	            update the EEI; EEI.Out-LIF = pointer to MPLS SWAP EEDB entry
 * 
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/cint_mpls_nop_action_encapsulation.c
 * cint
 * mpls_nop_action_info_run_dvapi(0,in_port,out_port);
 * exit;
 *
 * MPLS NULL ENTRY TEST
 * tx 1 psrc=in_port data=0x000000000011000035e7a2d38100000a884700d0512dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * 0x0011223344550000000000228100006488470006312cc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 */


struct mpls_nop_action_info_s {
   int in_vrf;  /* Core VRF */
   int in_port;  /* Ingress Port */
   int in_rif;
   int in_vsi; /* Ingress RIF */
   int in_encap_vlan; /*Ingress VLAN */
   int out_port; /* Egress port */
   int out_rif;
   int out_vsi;
   int out_encap_vlan;
   uint8 my_mac[6];/*InRIF my mac */
   uint8 my_sa[6]; /*OutRIF SA */
   uint8 l2_station_mask[6];
   bcm_gport_t gport_out;

   uint8 nh_da[6]; /* DA for 5.5.5.5 */

   bcm_mpls_label_t in_label;
   bcm_mpls_label_t out_label;
   bcm_mpls_label_t swap_label;
};

mpls_nop_action_info_s mpls_nop_action_info;

int mpls_nop_action_info_init() {
    uint8 my_mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 }; /*InRIF my mac */
    uint8 my_sa[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x22 }; /*OutRIF SA */
    uint8 l2_station_mask[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    uint8 nh_da[6] = { 00, 0x11, 0x22, 0x33, 0x44, 0x55 }; /* DA for 5.5.5.5 */


    mpls_nop_action_info.in_vrf = 3;  /* Core VRF */
    mpls_nop_action_info.in_port = 13;  /* Ingress Port */
    mpls_nop_action_info.in_rif;
    mpls_nop_action_info.in_vsi = 2000; /* Ingress RIF */
    mpls_nop_action_info.in_encap_vlan = 10; /*Ingress VLAN */

    mpls_nop_action_info.out_port = 14; /* Egress port */
    mpls_nop_action_info.out_rif;
    mpls_nop_action_info.out_vsi = 3000;
    mpls_nop_action_info.out_encap_vlan = 100;

    sal_memcpy(mpls_nop_action_info.my_mac, my_mac, 6);
    sal_memcpy(mpls_nop_action_info.my_sa, my_sa, 6);
    sal_memcpy(mpls_nop_action_info.l2_station_mask, l2_station_mask, 6);
    sal_memcpy(mpls_nop_action_info.nh_da, nh_da, 6);

    mpls_nop_action_info.in_label = 3333;
    mpls_nop_action_info.out_label = 5555;
    mpls_nop_action_info.swap_label = 99;

    mpls_nop_action_info.in_rif = mpls_nop_action_info.in_vsi;
    mpls_nop_action_info.out_rif = mpls_nop_action_info.out_vsi;
}


int mpls_nop_action_info_run(int unit) {
    int rv = 0;
    bcm_l2_station_t station;
    int station_id;
    bcm_vlan_port_t in_vlan_port;
    bcm_vlan_port_t out_vlan_port;
    bcm_l3_intf_t l3_in_if;
    bcm_l3_intf_t l3_out_if;
    create_l3_egress_s nh_l3_egress_object;
    bcm_mpls_egress_label_t label_stack;
    bcm_mpls_tunnel_switch_t entry;
    int mpls_encap_id_nop;
    int mpls_encap_id_swap;
    int ll_encap_id;
    bcm_if_t nh_fec;

    /* My Mac Setup */
    bcm_l2_station_t_init(&station);
    station.flags = 0;
    station.src_port_mask = 0;
    station.vlan_mask = 0;
    sal_memcpy(station.dst_mac_mask, mpls_nop_action_info.l2_station_mask, 6);
    sal_memcpy(station.dst_mac, mpls_nop_action_info.my_mac, 6);
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        return rv;
    }
    printf("L2 Station Added:.. StationId=%d\n", station_id);

    /* InLif for ingress VSI */
    bcm_vlan_port_t_init(&in_vlan_port);
    in_vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vlan_port.match_vlan  = mpls_nop_action_info.in_encap_vlan;
    in_vlan_port.egress_vlan = mpls_nop_action_info.in_encap_vlan;
    in_vlan_port.vsi  = mpls_nop_action_info.in_vsi;
    in_vlan_port.port = mpls_nop_action_info.in_port;
    rv = bcm_vlan_port_create(unit, &in_vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    printf("In Vlan Port Create: Vlan=%d, VSI=%d, Port-Id=0x%x\n", mpls_nop_action_info.in_encap_vlan, mpls_nop_action_info.in_vsi, in_vlan_port.vlan_port_id);

    /* Setup InRif for in packets */
    bcm_l3_intf_t_init(&l3_in_if);
    sal_memcpy(l3_in_if.l3a_mac_addr, mpls_nop_action_info.my_mac, 6);
    l3_in_if.l3a_flags = 0;
    l3_in_if.l3a_vid = mpls_nop_action_info.in_rif;
    l3_in_if.l3a_vrf = mpls_nop_action_info.in_vrf;
    l3_in_if.l3a_mtu = 1480;
    l3_in_if.l3a_ttl = 111;
    rv = bcm_l3_intf_create(unit, &l3_in_if); /* Returns l3if.l3a_intf_id  =  l3a_vid */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }
    printf("In L3 Intf Create: RIF = %d, VRV = %d\n", l3_in_if.l3a_intf_id, l3_in_if.l3a_vrf);

    /* Setup L3 Out-Rif for sending packet to be routed out to an intf */
    /* default VRF */
    bcm_l3_intf_t_init(&l3_out_if);
    sal_memcpy(l3_out_if.l3a_mac_addr, mpls_nop_action_info.my_sa, 6);
    l3_out_if.l3a_flags = 0;
    l3_out_if.l3a_vid = mpls_nop_action_info.out_rif;
    l3_out_if.l3a_mtu = 1480;
    l3_out_if.l3a_ttl = 191;
    rv = bcm_l3_intf_create(unit, &l3_out_if); /* Returns native_l3if.l3a_intf_id  =  l3a_vid */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }
    printf("In L3 Intf Create: RIF = %d, VRV = %d\n", l3_out_if.l3a_intf_id, l3_out_if.l3a_vrf);

    /* Setup OutLIf/InLif for the Egress VSI */
    bcm_vlan_port_t_init(&out_vlan_port);
    out_vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port.match_vlan  = mpls_nop_action_info.out_encap_vlan;
    out_vlan_port.egress_vlan = mpls_nop_action_info.out_encap_vlan;
    out_vlan_port.vsi  = mpls_nop_action_info.out_vsi;
    out_vlan_port.port = mpls_nop_action_info.out_port;
    rv = bcm_vlan_port_create(unit, &out_vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    printf("Out Vlan Port Create: Vlan=%d, VSI=%d, Port-Id=0x%x\n", mpls_nop_action_info.out_encap_vlan, mpls_nop_action_info.out_vsi, out_vlan_port.vlan_port_id);


    /* Setup the MPLS SWAP */
    bcm_mpls_egress_label_t_init(&label_stack);
    label_stack.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_stack.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    label_stack.label = mpls_nop_action_info.swap_label;
    label_stack.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    label_stack.l3_intf_id =  l3_out_if.l3a_intf_id;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label_stack);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    mpls_encap_id_swap = BCM_L3_ITF_VAL_GET(label_stack.tunnel_id);

    printf("MPLS SWAP GLEM = 0x%x\n", label_stack.tunnel_id);

    /* Setup the LL Encap and FEC */
    nh_l3_egress_object.vlan   = mpls_nop_action_info.out_vsi;
    nh_l3_egress_object.next_hop_mac_addr = mpls_nop_action_info.nh_da;
    nh_l3_egress_object.out_tunnel_or_rif = label_stack.tunnel_id;
    nh_l3_egress_object.out_gport   = mpls_nop_action_info.out_port;
    nh_l3_egress_object.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    nh_l3_egress_object.fec_id = nh_fec;

    rv = l3__egress__create(unit, &nh_l3_egress_object);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }
    printf("NH ARP GLEM = 0x%x , FEC = %x\n", nh_l3_egress_object.arp_encap_id, nh_fec);
    ll_encap_id = nh_l3_egress_object.arp_encap_id;
    nh_fec = nh_l3_egress_object.fec_id;

    /* Setup ILM entry */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform mode here */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    entry.label = mpls_nop_action_info.in_label;
    entry.egress_label.label = mpls_nop_action_info.out_label;
    entry.egress_if = nh_fec;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    printf("ILM entry set\n");

    /* Setup the MPLS NOP */
    bcm_mpls_egress_label_t_init(&label_stack);
    label_stack.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_stack.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    label_stack.action = BCM_MPLS_EGRESS_ACTION_NOP;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label_stack);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(mpls_encap_id_nop, label_stack.tunnel_id);

    printf("Allocated NOP MPLS entry with tunnel id = 0x%x \n", mpls_encap_id_nop);

    /* Adding MACT entry */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mpls_nop_action_info.my_mac, mpls_nop_action_info.out_vsi); 
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = mpls_nop_action_info.out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_add_l2_addr_to_gport\n");
        return rv;
    }


    /* PMF */
    bcm_field_group_config_t grp_config;
    bcm_field_aset_t aset;

    bcm_field_group_config_t_init(&grp_config);
    grp_config.group = 10;

    BCM_FIELD_QSET_INIT(grp_config.qset);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInPort);

    BCM_FIELD_ASET_INIT(grp_config.aset);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionVportNew);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionSystemHeaderSet);

    grp_config.priority = 10;
    grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp_config.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_config_create\n");
        return rv;
    }

    bcm_field_entry_t ent;
    rv = bcm_field_entry_create(unit, grp_config.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create\n");
        return rv;
    }
    rv = bcm_field_qualify_InPort(unit, ent, BCM_GPORT_SYSTEM_PORT_ID_GET(mpls_nop_action_info.in_port), 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_InPort\n");
        return rv;
    }

    /* Change outlif to NOP */
    rv = bcm_field_action_add(unit, ent, bcmFieldActionVportNew, mpls_encap_id_nop, 0);   
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_add\n");
        return rv;
    }

    int eei_value;
    if (is_device_or_above(unit,JERICHO)) {
        eei_value = (mpls_encap_id_swap & 0xFFFF)/*LSBs*/ + 
            ((mpls_encap_id_swap >> 16) << 20) + /*MSBs*/ 
            0xC00000 /*Valid bits and type*/ ;
    }
    else {
        eei_value = mpls_encap_id_swap + 0xF00000 /*Valid bits and type*/ ;
    }

    printf("EEI:%d\n",eei_value);

    rv = bcm_field_action_add(unit, ent, bcmFieldActionSystemHeaderSet,
                               bcmFieldSystemHeaderPphEei /* Modify the Eei system header */, eei_value);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_add\n");
        return rv;
    }

    rv = bcm_field_entry_prio_set(unit, ent, 100);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_prio_set\n");
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_install\n");
        return rv;
    }

    return rv;

}

int mpls_nop_action_info_run_dvapi(int unit, int in_port, int out_port) 
{

    mpls_nop_action_info_init();
    mpls_nop_action_info.in_port = in_port;  /* Ingress Port */
    mpls_nop_action_info.out_port = out_port;  /* Ingress Port */
    return mpls_nop_action_info_run(unit);

}
