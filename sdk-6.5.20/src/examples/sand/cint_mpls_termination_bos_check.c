/*~~~~~~~~~~~~~~~~~~~~~~~mpls termination with BoS check~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_termination_bos_check.c
 * Purpose: Example of mpls termination bos check. 
 *                In this example, it will demo mpls termination checking bos.
 * when configuring mpls termination with expecting bos option, the mpls label should be terminated only
 * if it's the bos. otherwise an unexpected no-bos will be recoginized..
 *  By default, trap strength for this event is 0, so the mpls termination still continue;
 *  Customer could set its strength as 4 (or larger value) so that this packet will be dropped.
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c  
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/sand/cint_mpls_termination_bos_check.c
 * cint
 * mpls_termination_add_with_bos_check_with_default(0,200,202); 
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000da3c4bca810000648847000c880400190904000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x000000000023000000000022810000c80800000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20000000 
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_mpls_termination_bos_check.c
 * cint
 * unexpcted_no_bos_termination_drop_set(0,1); 
 * exit;
 *
 * tx 1 psrc=200 data=0x0000000000110000da3c4bca810000648847000c880400190904000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *
 * tx 1 psrc=200 data=0x0000000000110000e37c126e810000648847000c890445000035000000000400d9ed0a0a0a0a64646464000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 0, Destination port: 0 
 *  Data: 0x000000000023000000000022810000c8080045000035000000000300daed0a0a0a0a64646464000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 */
int trap_id = 0;
int bos_check = 1;

int mpls_termination_double_tag_ac_lif_configuration(int unit,int in_port,int in_vlan){
    int rv = 0;
    int mpls_termination_label_index_enable;
	int mpls_termination_label_index_database_mode;

	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	mpls_termination_label_index_database_mode = soc_property_get(unit , "bcm886xx_mpls_termination_database_mode", 0);

	if ((mpls_termination_label_index_enable == 1) &&
		((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
		bcm_vlan_port_t vlan_port_1;
		 rv = bcm_vlan_control_port_set(unit, in_port, bcmVlanPortDoubleLookupEnable, 1);
		 if (rv != BCM_E_NONE) {
			printf("(%s) \n",bcm_errmsg(rv));
			return rv;
		 }

		 /* when a port is configured with  "bcmVlanPortDoubleLookupEnable" the VLAN domain must be unique in the device */
		rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_port_class_set %d\n", rv);
			return rv;
		}
		/* add port, according to port_vlan_vlan */
		bcm_vlan_port_t_init(&vlan_port_1);

		/* set port attributes, key <port-vlan-vlan>*/
		vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
		vlan_port_1.vsi = in_vlan;
		vlan_port_1.port = in_port;
		vlan_port_1.match_vlan = in_vlan;
		vlan_port_1.match_inner_vlan = 40;
		vlan_port_1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : in_vlan;
		vlan_port_1.egress_inner_vlan = 40;
		vlan_port_1.flags = 0;
		vlan_port_1.vlan_port_id = 0;

		rv = bcm_vlan_port_create(unit, &vlan_port_1);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_vlan_port_create\n");
			return rv;
	    }

	}
	return rv;
}

int mpls_termination_add_with_bos_check_with_default(int unit,int in_port, int out_port){
    int rv = 0;
    bcm_pbmp_t pbmp, ubmp;
    bcm_mac_t my_in_mac_addr =  {0, 0, 0, 0, 0, 0x11} ;
    bcm_mac_t my_out_mac_addr ={0, 0, 0, 0, 0, 0x22} ;
    bcm_mac_t my_nh_mac_addr = {0, 0, 0, 0, 0, 0x23};
    int in_vlan = 100;
    int out_vlan = 200;
    int label_index_enable;

	/* configure the AC lif for double tag packets - this function relevant for termination mode 22-23 only */
	rv  = mpls_termination_double_tag_ac_lif_configuration(unit,in_port,in_vlan);
    if (rv != BCM_E_NONE) {
       printf("Error, in mpls_termination_double_tag_ac_lif_configuration\n");
       return rv;
    }

    /* Create the VLAN (in_lif/in_vsi here)*/
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);

    print bcm_vlan_destroy(unit, in_vlan);
    print bcm_vlan_create(unit, in_vlan);
    print bcm_vlan_port_add(unit, in_vlan, pbmp, ubmp);

    /* Used for L2 termination (ingress) and L2 encapsulation (egress) */
    bcm_l3_intf_t l3_ingress;
    bcm_l3_intf_t_init(&l3_ingress);

    l3_ingress.l3a_ttl = 30;
    l3_ingress.l3a_vrf=1;
    l3_ingress.l3a_vid = in_vlan;
    sal_memcpy(l3_ingress.l3a_mac_addr, my_in_mac_addr, 6);
    rv = bcm_l3_intf_create(unit, &l3_ingress);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_l3_intf_create for creating L3 intf!\n", rv);
        return rv;
    }

    int flags = 0;
    int tunnel_id;
    bcm_mpls_tunnel_switch_t label1;

    bcm_mpls_tunnel_switch_t_init(&label1);
    label1.action = BCM_MPLS_SWITCH_ACTION_POP;
    if (bos_check) {
        label1.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
    }
    label1.label = 0xc8;
    /* read mpls index soc property */
    label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(&label1.label, 0xc8, 1);
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /** In JR2, tunnel_if was not used in this api.*/
        label1.tunnel_if = 888;
    }
    label1.tunnel_id = tunnel_id;
    label1.vpn = 1; 
    rv = bcm_mpls_tunnel_switch_create(unit,&label1);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create for creating MPLS POP entry!\n", rv);
        return rv;
    }

    /* Used for L2 termination (ingress) and L2 encapsulation (egress) */
    create_l3_intf_s  pwe_rif;
    bcm_if_t l3_intf_id;

    pwe_rif.ttl_valid = 1;
    pwe_rif.ttl = 30;
    pwe_rif.vrf_valid = 1;
    pwe_rif.vrf = 1;
    pwe_rif.vsi = 888;
    pwe_rif.my_global_mac = my_in_mac_addr;
    pwe_rif.my_lsb_mac = my_in_mac_addr;
    rv = l3__intf_rif__create(unit, &pwe_rif);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in l3__intf_rif__create for creating l3 interface!\n", rv);
        return rv;
    }

    /*****************************Egress*****************************/

    /* Create the VLAN out_lif/out_vsi*/
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, out_port);
    print bcm_vlan_destroy(unit, out_vlan);
    print bcm_vlan_create(unit, out_vlan);
    print bcm_vlan_port_add(unit, out_vlan, pbmp, ubmp);

    /* Used for L2 termination (ingress) and L2 encapsulation (egress) */
    bcm_l3_intf_t l3_egress;
    bcm_l3_intf_t_init(&l3_egress);

    l3_egress.l3a_ttl = 30;
    l3_egress.l3a_vrf = 5; 
    l3_egress.l3a_vid = out_vlan;
    sal_memcpy(l3_egress.l3a_mac_addr, my_out_mac_addr, 6);
    rv = bcm_l3_intf_create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_l3_intf_create for creating l3 interface!\n", rv);
        return rv;
    }

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);
    l3eg.flags = BCM_L3_EGRESS_ONLY;
    sal_memcpy(l3eg.mac_addr, my_nh_mac_addr, 6);
    l3eg.vlan = out_vlan;
    l3eg.port = out_port;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_l3_egress_create for creating ARP entry!\n", rv);
        return rv;
    }

    printf("l3 egress objects create, arp_encap_id = 0x%08X\n", l3eg.encap_id);

    /*Create fec, using encap-id allocated as part of LL(EEDB entry) creation*/
    bcm_l3_egress_t fec;
    bcm_if_t fecid;
    bcm_l3_egress_t_init(&fec);

    /** In JR2, if only one outlif/outrif exists, if should reside in intf.*/
    if (!is_device_or_above(unit, JERICHO2)) {
        fec.encap_id = l3eg.encap_id;
    } else {
        fec.intf = l3_egress.l3a_intf_id;
    }
    fec.vlan = out_vlan;
    fec.port = out_port;

    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &fec, &fecid);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_l3_egress_create for creating FEC entry!\n", rv);
        return rv;
    }

    printf("l3 egress objects create, fec_id = 0x%08X\n", fecid);

    /*Add an ILM entry with label 400, pointing valid FEC*/
    bcm_mpls_tunnel_switch_t_init(&label1);
    label1.action = BCM_MPLS_SWITCH_ACTION_PHP;
    label1.label = 0x190;
    label1.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    if (!is_device_or_above(unit, JERICHO2)) {
        /** In JR2, the next protocol is resolved according to the next nibble by hw.*/
        label1.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
    }
    /* read mpls index soc property */
    label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(&label1.label,0x190,2);
    }

    label1.egress_if = fecid;
    rv = bcm_mpls_tunnel_switch_create(unit,&label1);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create for creating MPLS PHP entry\n!", rv);
        return rv;
    }


    uint32_t ip_addr = 0x64646400;
    uint32_t mask = 0xffffff00;
    bcm_l3_route_t route_t;

    bcm_l3_route_t_init(&route_t);
    route_t.l3a_subnet = ip_addr;
    route_t.l3a_ip_mask = mask;
    route_t.l3a_vrf = 1;
    route_t.l3a_intf = fecid;
    bcm_stk_modid_get(unit, &route_t.l3a_modid);
    rv = bcm_l3_route_add(unit, &route_t);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_l3_route_add for creating host entry!\n", rv);
        return rv;
    }

    /*create the trap id for unpected no bos packet*/
    if (bos_check) {
        rv = bcm_rx_trap_type_create(unit,0,bcmRxTrapMplsUnexpectedNoBos,&trap_id);
        if(rv != BCM_E_NONE)
        {
            printf("Error, in trap create, trap type: %d \n", bcmRxTrapMplsUnexpectedNoBos);
            return rv;
        }
    }

    return rv;
}

int unexpcted_no_bos_termination_drop_set(int unit,int drop){
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    bcm_rx_trap_config_t_init(&config);

    config.trap_strength = drop? 4 : 0;
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
   
    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap code set, trap id: %d \n", trap_id);
        return rv;
    } 

    return rv; 
}

