/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* 
 * The below CINT demonstrates TWAMP.
 * Available for 88660_A0 and above.
 *
 *
 * How to run: 
 * load OAM soc properties
 * 
 * cint utility/cint_utils_multicast.c
 * cint utility/cint_utils_vlan.c
 * cint utility/cint_utils_l3.c
 * cint cint_ip_route.c
 * cint cint_twamp.c 
 * cint 
 * int unit = 0; 
 * int port = 200;
 * int vlan = 10;
 * twamp_example(unit,vlan,port); 
 */

/**
 * TWAMP example
 * Creating l3 interface, trap and two PMF rules 
 *  
 * Local IP adress is 20.0.0.2 
 * Remote IP adress is 10.0.0.2 
 *  
 * PMF Rule 1 will catch 20.0.0.2 -> 10.0.0.2 IP traffic where 
 * UDP dest port is 862, stamp timestamp on TWAMP header and 
 * send to network 
 *  
 * PMF Rule 2 will catch 10.0.0.2 -> 20.0.0.2 IP traffic where 
 * UDP dest port is 862, stamp timestamp LSB on OTSH and 
 * timestamp MSB prior to first network header (Ethernet) 
 * and trap to CPU 
 *  
 * @author Aviv (31/05/2015)
 * 
 * @param unit
 * @param in_vlan
 * @param in_sysport
 * @param in_vlan2
 * @param in_sysport2
 *  
 * @return int 
 */
int twamp_example(int unit, int in_vlan, int in_sysport, int in_vlan2, int in_sysport2) {
    int rv;
    bcm_field_group_config_t grp;
    bcm_field_entry_t entry1;
    bcm_field_entry_t entry2;

    int trap_strength = 7;

    int trap_id = 0;
    bcm_gport_t gport_trap1 = 0;
    bcm_rx_trap_config_t config;

    bcm_mac_t port_1_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x88};  /* my-MAC */
	bcm_mac_t port_2_mac_address  = {0x00, 0x0c, 0x00, 0x01, 0x00, 0x88};  /* my-MAC */
    bcm_mac_t port_1_next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
	bcm_mac_t port_2_next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0xad, 0x1d};

    uint32 local_ip_addr = 0x14000002; /* 20.0.0.2 */
    uint32 remote_ip_addr = 0x0a000002; /* 10.0.0.2 */

    /*** create ingress router interface for (Arad+ -> CPU) scenario ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
        return rv;
    }
    create_l3_intf_s ingress_intf;
    ingress_intf.vsi = in_vlan;
    ingress_intf.my_global_mac = port_1_mac_address;
    ingress_intf.my_lsb_mac = port_1_mac_address;
    rv = l3__intf_rif__create(unit, &ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** the ingress interfcae will also act as egress for (CPU -> Arad+) scenario ***/
    /*** create fec ***/
    create_l3_egress_s l3_egress;
    l3_egress.out_gport = in_sysport;
    l3_egress.vlan = in_vlan;
    l3_egress.next_hop_mac_addr = port_1_next_hop_mac;
    l3_egress.out_tunnel_or_rif = ingress_intf.rif;
    
    rv = l3__egress__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", in_sysport, unit);
    }
    bcm_if_t l3egid = l3_egress.fec_id;
    printf("FEC created: %d\n",l3egid);

    /*** create l3 route for remote ip address in case 1 ***/
    rv = add_route(unit, remote_ip_addr, 0xffffffff, 0, l3egid);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route\n");
        return rv;
    }

    /*** create ingress router interface for (CPU -> Arad+) scenario ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan2, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan2, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan2, in_sysport2, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport2, in_vlan2);
        return rv;
    }
    create_l3_intf_s ingress_intf2;
    ingress_intf2.vsi = in_vlan2;
    ingress_intf2.my_global_mac = port_2_mac_address;
    ingress_intf2.my_lsb_mac = port_2_mac_address;
    rv = l3__intf_rif__create(unit, &ingress_intf2);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

	/*** create fec ***/
    create_l3_egress_s l3_egress2;
    l3_egress2.out_gport = in_sysport2;
    l3_egress2.vlan = in_vlan2;
    l3_egress2.next_hop_mac_addr = port_2_next_hop_mac;
    l3_egress2.out_tunnel_or_rif = ingress_intf2.rif;
    
    rv = l3__egress__create(unit, &l3_egress2);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", in_sysport2, unit);
    }
    bcm_if_t l3egid2 = l3_egress2.fec_id;
    printf("FEC created: %d\n",l3egid2);

	/*** create l3 route for remote ip address in case 2 ***/
	rv = add_route(unit, local_ip_addr, 0xffffffff, 0, l3egid2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route\n");
        return rv;
    }
    
    /*** create trap ***/
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    bcm_rx_trap_config_t_init(&config);
    config.flags = (BCM_RX_TRAP_UPDATE_DEST |
                   BCM_RX_TRAP_UPDATE_PRIO |
                   BCM_RX_TRAP_TRAP);
    config.dest_port = 0;
    config.prio = 0;

    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /*** create PMF rule for (Arad+ -> CPU) scenario ***/
    BCM_GPORT_TRAP_SET(gport_trap1, trap_id, trap_strength, 0);

    bcm_field_group_config_t_init(&grp);
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    grp.priority = 30;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL4SrcPort);

    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionTrap);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionOam);

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_entry_create(unit, grp.group, &entry1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_DstIp(unit, entry1, local_ip_addr, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_SrcIp(unit, entry1, remote_ip_addr, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_L4DstPort(unit, entry1, 862, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    /* Offset is 0x20 */
    rv = bcm_field_action_add(unit, entry1, bcmFieldActionOam, 0xc020, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_action_add(unit,entry1,bcmFieldActionTrap,gport_trap1,0x1234);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_entry_install(unit, entry1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /*** create PMF rule for (CPU -> Arad+) scenario ***/
    bcm_field_group_config_t_init(&grp);
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    grp.priority = 30;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL4SrcPort);

    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionOam);

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_entry_create(unit, grp.group, &entry2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_DstIp(unit, entry2, remote_ip_addr, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_SrcIp(unit, entry2, local_ip_addr, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_qualify_L4DstPort(unit, entry2, 862, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    /* Offset is 0x34 */
    rv = bcm_field_action_add(unit, entry2, bcmFieldActionOam, 0xc034, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    rv = bcm_field_entry_install(unit, entry2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    return rv;
}
