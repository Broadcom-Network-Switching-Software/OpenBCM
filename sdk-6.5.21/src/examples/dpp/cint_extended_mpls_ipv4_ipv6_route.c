/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*  Note: this cint also includes tests for 4 label push and protection*/

/* ********* 
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 1;
int L3_uc_rpf_mode = bcmL3IngressUrpfLoose;
/* Should be set if the SOC property bcm886XX_l3_ingress_urpf_enable is set. */
int urpf_mode_per_rif = 0;

/* Used outside basic_example function */
int default_intf = 0;      
int default_vrf = 0;

int default_mtu = 0;
int default_mtu_forwarding = 0;

/* 4 label push control parameters */
int is_protection = 0;
int is_4_label_push=0;


/**********
  functions
 */

int mpls_lsr_get_special_mode(){
	printf("SPAG 4 labels push mode - Get:%d\n",is_4_label_push);
	return is_4_label_push;
}

void mpls_lsr_set_special_mode( int val)
{
	printf("SPAG 4 labels push mode - Set:%d\n",val);
	is_4_label_push = val;
	if( is_4_label_push )
		mpls_tunnel_label_out = 600;
	else
		mpls_tunnel_label_out = 0;
}
 
/* configure the mpls lsr for 4 label push feature
 * this function overide the function in cint_mpls_lsr.c
 */
int
mpls_lsr_config(int *units_ids, int nof_units, int extend_example){

	int rv, i;
    int unit;
	bcm_field_group_t grp;
	int entry;

    printf("SPAG mode TEST: config new LSR: mpls_lsr_config\n");
	printf("SPAG: This functions overrides default function in SPAG mode\n");
    rv = mpls_lsr_config_init( units_ids, nof_units, extend_example );
    if (rv != BCM_E_NONE) {
    	printf("Error, in mpls_lsr_config_init\n");
    	return rv;
    }
	
	/*creating group for entry management */
	rv = field_extended_ip_mpls_group_create(unit, &grp);
	if (BCM_E_NONE != rv) {
		printf("Error in field_extended_ip_mpls_group_create\n");
		return rv;
	}
	printf("field_extended_ip_mpls_group_create\n");

	/*adding entries to the KBP tables*/
	if (is_protection) {
		rv = field_entry_add_mpls_4_labels_protection(unit, grp, &entry);
	}
	else
	{
		rv = field_entry_add_mpls_4_labels(unit, grp, &entry);
	}

	if (BCM_E_NONE != rv) {
		printf("Error in field_entry_add\n");
		return rv;
	}
	printf("field_entry_add %d\n", entry);

	return rv;
}


/*
 * packet will be routed from in_sysport to out-sysport
 * HOST:
 * packet to send:
 *  - in port = in_sysport
 *  - vlan 10.
 *  - DA = 00:00:00:02:00:01 {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x7fffff03 (127.255.255.03) or 0x7ffffa00 (127.255.250.0) - 0x7ffffaff (127.255.250.255)
 * expected:
 *  - out system port = out_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  TTL decremented
 *
 * Route:
 * packet to send:
 *  - in port = in_sysport
 *  - vlan = 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x78ffff03 (120.255.255.3) or 0x78fffa00 (120.255.250.0) - 0x78fffaff (120.255.250.255)
 * expected:
 *  - out system port = out_sysport
 *  - vlan = 10.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 int units_ids[2];
 int nof_units = 1;
 int in_sysport = 13;
 int out_sysport = 13;
 int urpf_mode = bcmL3IngressUrpfStrict;
 print example_rpf(units_ids, nof_units, in_sysport, out_sysport, urpf_mode);
 *
 * in SPAG mode ( protection enabled )This fuction overide the example_rpf in the cint_ip_route_rpf.c
 */
int example_rpf(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode){
    int rv = 0, i;
    int unit, flags;
    int fec[2] = {0x0,0x0};
    int itf_flags = BCM_L3_RPF;
    int egr_flags = 0;
    int vrf = 0;

    int host_l = 0x78ffff03; /*120.255.255.3*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;

    int host_r = 0x7fffff03; /*127.255.255.3*/
    int subnet_r = 0x7ffffa00; /*127.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    int encap_id[2]={0x0,0x0};

	int entry;
	bcm_field_group_t grp;

    create_l3_intf_s intf;

	printf("SPAG: This functions overrides default function in SPAG mode\n");
    /* set global definition defined in cint_ip_route.c */
    /* this way all UC-rpf checks is done according to this config */
    L3_uc_rpf_mode  = urpf_mode;

    rv = l3_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps \n");
    }

    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
	for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
	    
	    rv = vlan__open_vlan_per_mc(unit, vlan_l, 0x1);  
	    if (rv != BCM_E_NONE) {
	    	printf("Error, open_vlan=%d, in unit %d \n", vlan_l, unit);
	    }
	    rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
	    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
	    	printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
	      return rv;
	    }
	    
	    intf.vsi = vlan_l;
        intf.my_global_mac = mac_address_l;
        intf.my_lsb_mac = mac_address_l;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
	    
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_l = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, vlan_r, 0x1);  
		if (rv != BCM_E_NONE) {
			printf("Error, open_vlan=%d, in unit %d \n", vlan_r, unit);
		}
		rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
		if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
			printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
		  return rv;
		}

		intf.vsi = vlan_r;
        intf.my_global_mac = mac_address_r;
        intf.my_lsb_mac = mac_address_r;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_r = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, mac_address_next_hop_r , 6);
        l3eg.allocation_flags = flags;
        l3eg.l3_flags = flags;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = vlan_r;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];
		
        if (is_protection==0) {
            l3eg.out_tunnel_or_rif = ing_intf_r;
            rv = l3__egress__create(unit,&l3eg);
        }
        else{
            l3eg.out_tunnel_or_rif = 0;
            rv = l3__egress__create(unit,&l3eg);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id; 

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg1;
        sal_memcpy(l3eg1.next_hop_mac_addr, mac_address_next_hop_l , 6);
        l3eg1.allocation_flags = flags;
        l3eg1.l3_flags = flags;
        l3eg1.out_tunnel_or_rif = ing_intf_l;
        l3eg1.out_gport = in_sysport;
        l3eg1.vlan = vlan_l;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }
        
        fec[1] = l3eg1.fec_id;
        encap_id[1] = l3eg1.arp_encap_id; 

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[1]);
            printf("next hop mac at encap-id %08x, \n", encap_id[1]);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /* create protected entry */
	rv = field_extended_ip_mpls_group_create(unit, &grp);
	if (BCM_E_NONE != rv) {
		printf("Error in field_extended_ip_mpls_group_create\n");
		return rv;
	}
	printf("field_extended_ip_mpls_group_create\n");
	rv = field_ip_rpf_entry_add(unit, grp, &entry);
	if (BCM_E_NONE != rv) {
		printf("Error in field_entry_add\n");
		return rv;
	}
	printf("field_entry_add %d\n", entry);

	return rv;
}

/* 
Tests protection. 
 Packet will be routed from in_port to out-port
packet to send:
  - in port = in_port
  - vlan 15.
  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
  - DIP = 0x7fffff03 (127.255.255.03)
expected: 
  - out port = out_port
  - vlan 100.
  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
  - MPLS label: label 300
 */
int basic_example_ip_protection(int *units_ids, int nof_units, int in_sysport, int out_sysport, int is_extender){
    int rv;
    int i, unit;
    int ing_intf_in; 
	int ing_intf_out; 
    int fec[2] = {0x0,0x0};      
    int flags;
    int flags1;
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 3;
    int host;
    int encap_id[2]={0x0,0x0};
    int route;
    int mask; 
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac2 */
	int rc=0;

    create_l3_intf_s intf;

    /* Port 201 is set as port extender for the testing. Call bcm_port_control_set to enable */
    if (is_extender) {
        printf("Enabling port %d as port extender, bcm_port_control_set\n", out_sysport); 
        rv = bcm_port_control_set(0,out_sysport,bcmPortControlExtenderEnable,1); 
        if (rv != BCM_E_NONE) {
            printf("Error enabling port port %d as port extender, bcm_port_control_set, exit 60\n",out_sysport);
            return rv;
        }

    }

    /*PMF configuration for 4-label push and pritection*/
	int a;
	rc = field_acl_4_label_push_groups_set(0, 1, 2, 3, 4, 5, 6, 7, &a);
	if (rc != BCM_E_NONE) {
	printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);
	return rc;
	}

	/*Add protection entry*/
	int e_ip, e_mpls;
	uint16 qual_data = 0x388;
	rc = field_acl_4_label_push_protection_pointer_entries_add(0,1,2,a, qual_data, &e_ip, &e_mpls);
	if (rc != BCM_E_NONE) {
	printf ("field_acl_4_label_push_protection_pointer_entries_add failed: %d \n", rc);
	return rc;
	}

    /*** create ingress router interface ***/
    flags = 0;
    ing_intf_in = 0;
    /*units_array_make_local_first(units_ids, nof_units, in_sysport);*/
    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
	    
	    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
	    if (rv != BCM_E_NONE) {
	    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
	    }
	    rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
	    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
	    	printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
	      return rv;
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
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
    /*units_array_make_local_first(units_ids,nof_units,out_sysport);*/
    for (i = 0 ; i < nof_units ; i++) {
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
    /* out_sysport unit is already first */
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.allocation_flags = flags1;
        l3eg.l3_flags = flags1;
        l3eg.out_tunnel_or_rif = 0;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }
        
        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }

    /*** create egress object 2***/
    /* out_sysport unit is already first */
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg1;
        sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg1.allocation_flags = flags1;
        l3eg1.l3_flags = flags1;
        l3eg1.out_tunnel_or_rif = ing_intf_out;
        l3eg1.out_gport = out_sysport;
        l3eg1.vlan = out_vlan;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }
        
        fec[1] = l3eg1.fec_id;
        encap_id[1] = l3eg1.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
    }

    /*** add route point to FEC2 ***/
    int result = 0;
    bcm_field_group_t grp;
    int entry;

    /*  result = field_extended_ip_mpls_group_create(unit, &grp);    */
    result = field_extended_ip_mpls_group_create_if_not_exist(unit,&grp);
    if (BCM_E_NONE != result) {
        printf("Error in field_extended_ip_mpls_group_create\n");
        return result;
    }
    
    printf("field_extended_ip_mpls_group_create\n");
    result = field_entry_add_test(unit, grp, &entry);
    if (BCM_E_NONE != result) {
        printf("Error in field_entry_add\n");
        return result;
    }
    printf("field_entry_add %d\n", entry);

    default_intf = fec[1];
    default_vrf = vrf;

    return rv;
}


/*
 * packet will be routed from in_sysport to out-sysport with protection state
 * HOST: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan 10.
 *  - DA = 00:00:00:02:00:01 {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x7fffff03 (127.255.255.03) or 0x7ffffa00 (127.255.250.0) - 0x7ffffaff (127.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - MPLS label 300
 *  
 * Route: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan = 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x78ffff03 (120.255.255.3) or 0x78fffa00 (120.255.250.0) - 0x78fffaff (120.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan = 10.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - MPLS label 300
 */
int example_rpf_protection(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode, int pmf_config){
	is_protection=1;
	int rc=0;

	/*PMF configuration for 4-label push and pritection*/
	int a;
	if(pmf_config) {
		rc = field_acl_4_label_push_groups_set(0, 1, 2, 3, 4, 5, 6, 7, &a); 
		if (rc != BCM_E_NONE) {
		printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);
		return rc;
		}
	
		/*Add protection entry*/
		int e_ip, e_mpls;
		uint16 qual_data = 0x388;
		rc = field_acl_4_label_push_protection_pointer_entries_add(0,1,2,a, qual_data, &e_ip, &e_mpls);
		if (rc != BCM_E_NONE) {
		printf ("field_acl_4_label_push_protection_pointer_entries_add failed: %d \n", rc);
		return rc;
		}
	}
	return example_rpf(units_ids, nof_units, in_sysport, out_sysport, urpf_mode);
}


/* 
    tests 4 label push or protection 
	send packet with ethernet header with DA 0:0:0:0:55:34 and vlan tag id 17
    and IPV6 header with DIP  01001600350070000000db0700000000    
	packets will arrive at out_port with: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:0:0:0:55:34 and vlan tag id 18
    and 4 MPLS labels: eth o MPLS(600) o MPLS(200) o MPLS(300) o MPLS(250) o IP
    if is_protection=1 so ppp state=1 else ppp state=0
*/

int bcm_l3_ip6_4_label_push_protection(int unit,  int in_port, int out_port, int ttl_threshold, int is_protection, int install_field_groups) {

  int CINT_NO_FLAGS = 0;
  int vrf = 3;
  int rc;
  int in_vlan = 17;
  int out_vlan = 18;
  int nof_mc_members = 2;
  int ipmc_index[MAX_PORTS] = {6010, 6011};

  int ingress_intf[MAX_PORTS];
  int egress_intf[MAX_PORTS];
  int encap_id[MAX_PORTS];
  bcm_multicast_t mc_id[MAX_PORTS];

  /* used for multi device: */
  int second_ingress_intf[MAX_PORTS];
  int ingress_port_list[MAX_PORTS];
  int egress_port_list[MAX_PORTS];

  bcm_if_t cud;
  int trap_id, port_ndx;
  bcm_rx_trap_config_t trap_config;

  bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

  int tunnel_id = 0;
  bcm_field_group_t grp;
  int entry;

  mpls__egress_tunnel_utils_s mpls_tunnel_properties;

  create_l3_intf_s intf;

  /*PMF configuration for 4-label push and pritection*/
  int a;
  if (install_field_groups) {
	  rc = field_acl_4_label_push_groups_set(unit, 1, 2, 3, 4, 5, 6, 7, &a);
	  if (rc != BCM_E_NONE) {
		  printf("field_acl_4_label_push_groups_set failed: %d \n", rc);
		  return rc;
	  }
  }
  if (is_protection == 1) {
	  /*Add protection entry*/
	  int e_ip, e_mpls;
	  uint16 qual_data = 0x388;
	  rc = field_acl_4_label_push_protection_pointer_entries_add(0, 1, 2, a, qual_data, &e_ip, &e_mpls);
	  if (rc != BCM_E_NONE) {
		  printf("field_acl_4_label_push_protection_pointer_entries_add failed: %d \n", rc);
		  return rc;
	  }
  }


  /* create in-rif */
  /* TTL/hop-limit threshold in valid only only in the egress, so for in-rif it is set to max - 255 */
  rc = bcm_vlan_create(unit, in_vlan);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
    printf("failed to create vlan %d", in_vlan);
  }

  rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
  if (rc != BCM_E_NONE) {
    printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
    return rc;
  }

  intf.vsi = in_vlan;
  intf.my_global_mac = mac_l3_ingress;
  intf.my_lsb_mac = mac_l3_ingress;
  intf.vrf_valid = 1;
  intf.vrf = vrf;
  intf.ttl_valid = 1;
  intf.ttl = 255;
  intf.qos_map_valid = 1;
  intf.qos_map_id = qos_map_id_l3_ingress_get(unit);

  rc = l3__intf_rif__create(unit, &intf);
  ingress_intf[0] = intf.rif;
  if (rc != BCM_E_NONE) {
    printf("Error, l3__intf_rif__create");
  }

  /* create out-rif */
  rc = bcm_vlan_create(unit, out_vlan);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
	  printf("failed to create vlan %d", out_vlan);
  }

  rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
  if (rc != BCM_E_NONE) {
	  printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
	  return rc;
  }

  intf.vsi = out_vlan;
  intf.ttl = ttl_threshold;

  rc = l3__intf_rif__create(unit, &intf);
  ingress_intf[1] = intf.rif;
  if (rc != BCM_E_NONE) {
	  printf("Error, l3__intf_rif__create");
  }

  /*create tunnel 4label push*/
  if (is_protection==0) {
	  mpls_tunnel_properties.label_in = 200;
	  mpls_tunnel_properties.label_out = 600;
	  mpls_tunnel_properties.next_pointer_intf = ingress_intf[1];
	
	  printf("Trying to create tunnel initiator\n");
	  rc = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
	  if (rc != BCM_E_NONE) {
         printf("Error, in mpls__create_tunnel_initiator__set\n");
         return rc;
      }

	  tunnel_id = mpls_tunnel_properties.tunnel_id;
  }
  
  /* create fec for UC IPV6 entry */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, mac_l3_egress , 6);
    l3eg.allocation_flags = CINT_NO_FLAGS;
    l3eg.l3_flags = CINT_NO_FLAGS;
    l3eg.out_tunnel_or_rif = tunnel_id;
    l3eg.out_gport = out_port;
    l3eg.vlan = out_vlan;
    l3eg.fec_id = egress_intf[0];
    l3eg.arp_encap_id = encap_id[0];

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    egress_intf[0] = l3eg.fec_id;
    encap_id[0] = l3eg.arp_encap_id;

  /* Add vlan member to each outgoing port */
  for(port_ndx = 0; port_ndx < nof_mc_members; ++port_ndx) {        
	rc = bcm_vlan_gport_add(unit, out_vlan, out_port+port_ndx, 0);
	if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", out_port+port_ndx, out_vlan);
	  return rc;
	}
  }


    /*Add route entry to the kbp*/
    rc = field_extended_ipv6_group_create(unit, &grp);
    if (BCM_E_NONE != rc) {
        printf("Error in field_extended_ip_mpls_group_create\n");
        return rc;
    }
    printf("field_extended_ip_mpls_group_create\n");

	if (is_protection==1) {
        rc = field_ipv6_entry_add_protection_test(unit, grp, &entry); 
        if (BCM_E_NONE != rc) {
            printf("Error in field_entry_add\n");
            return rc;
        }
        printf("field_entry_add %d\n", entry);
	}
    else
    {
        rc = field_ipv6_entry_add_test(unit, grp, &entry);
        if (BCM_E_NONE != rc) {
            printf("Error in field_entry_add\n");
            return rc;
        }
        printf("field_entry_add %d\n", entry);
    }

  return 0;
}


/* tests: four label push and protection
   packet comes in with IPv4oMPLS1000oMPLS400oEth
   First MPLS is terminated (400)
   Second MPLS is swapped (1000->2000)
   The eei2 add additional MPLS header (300)
   Then encapsulation add additional MPLS header (200)(600)
   Packet goes out as IPv4oMPLS2000oMPLS300oMPLS200oMPLS600oEth
 
   is_protection = 1 for pratection mode
   is_protection = 0 for non protection mode
 
    */ 
 
int 
four_label_push_basic_example(int *units_ids, int nof_units, int in_sysport, int out_sysport, int protection, int create_field_groups){
    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000; /*change*/
    int out_label  = 2000;
    int in_vid = 10;
    int eg_vid = 10;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int rc =0;

    /*PMF configuration for 4-label push and pritection*/
    int a;
	if (create_field_groups) {
		rc = field_acl_4_label_push_groups_set(0, 1, 2, 3, 4, 5, 6, 7, &a); 
		if (rc != BCM_E_NONE) {
			printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);
		return rc;
		}
	}

    printf ("field_acl_4_label_push_groups_set%d \n", rc);

	if (protection) {
        /*Add protection entry*/
        int e_ip, e_mpls;
        uint16 qual_data = 0x388;
        rc = field_acl_4_label_push_protection_pointer_entries_add(0,1,2,a, qual_data, &e_ip, &e_mpls);
        if (rc != BCM_E_NONE) {
        	printf ("field_acl_4_label_push_protection_pointer_entries_add failed: %d \n", rc);
        return rc;
        }
	}
    
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* create vlan*/

	if (protection) {
		is_protection=1;
	}
	mpls_lsr_set_special_mode(1);
	mpls_lsr_config(units_ids, nof_units, 0); 
}

/* 
 * Tests 4 label push 
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
 *  MPLS label: label 600,
 *  MPLS label: label 200,
 *  MPLS label: label 300, 
 *  MPLS label: label 250, 
 */
int example_ip_to_tunnel_4_label_push(int *units_ids, int nof_units, int in_sysport, int out_sysport, int create_pmf_groups){
    int rv;
    int unit, i;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};      
    int flags;
    int flags1;
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0};
    int route;
    int mask; 
    int tunnel_id = 0;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int result = 0;
    bcm_field_group_t grp;
    int entry;
    int rc=0;

	mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    create_l3_intf_s intf;

    /*PMF configuration for 4-label push and pritection*/
	if (create_pmf_groups) {
		int a; 
		rc = field_acl_4_label_push_groups_set(0, 1, 2, 3, 4, 5, 6, 7, &a);
		if (rc != BCM_E_NONE) {
		printf ("field_acl_4_label_push_groups_set failed: %d \n", rc);
		return rc;
		}
	}


    /*** create ingress router interface ***/
    flags = 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
		if (rv != BCM_E_NONE) {
			printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
		}
		rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
		if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
			printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
		  return rv;
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
    }

	/*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
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

		mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 600;
		mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
	
		printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
			printf("Error, in mpls__create_tunnel_initiator__set\n");
			return rv;
		}
		/* having a tunnel id != 0 is equivalent to being WITH_ID*/

		tunnel_id = mpls_tunnel_properties.tunnel_id;
    }

    /*** Create egress object1 with the tunnel_id ***/
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.allocation_flags = flags1;
        l3eg.l3_flags = flags1;
        l3eg.out_tunnel_or_rif = tunnel_id;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }

    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        create_l3_egress_s l3eg1;
        sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg1.allocation_flags = flags1;
        l3eg1.l3_flags = flags1;
        l3eg1.out_tunnel_or_rif = tunnel_id;
        l3eg1.out_gport = out_sysport;
        l3eg1.vlan = out_vlan;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[1] = l3eg1.fec_id;
        encap_id[1] = l3eg1.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
    }

    /*** add route to KBP point to FEC2 ***/

    /*  result = field_extended_ip_mpls_group_create(unit, &grp);    */
    result = field_extended_ip_mpls_group_create_if_not_exist(unit,&grp);
    if (BCM_E_NONE != result) {
        printf("Error in field_extended_ip_mpls_group_create\n");
        return result;
    }

    printf("field_extended_ip_mpls_group_create\n");
    result = field_entry_add_ip_4_labels(unit, grp, &entry);
    if (BCM_E_NONE != result) {
        printf("Error in field_entry_add\n");
        return result;
    }
    printf("field_entry_add %d\n", entry);
       
    return result;

    return rv;
}

/* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
int
create_tunnel_initiator_4_label_push(int unit, int* ingress_intf, int *tunnel_id) {
    bcm_mpls_egress_label_t label_array[2];
    int num_labels;
    int rv;

    /*extra label push- label 200*/
    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].exp = exp;
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = 200;
    label_array[0].ttl = 20;
    label_array[0].l3_intf_id = *ingress_intf;
    label_array[0].tunnel_id = *tunnel_id;
    num_labels = 1;

    /*extra label push- label 600*/
	bcm_mpls_egress_label_t_init(&label_array[1]);
	label_array[1].exp = exp;
	label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
	if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
		label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
	} else {
		label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
	}
	label_array[1].label = 600;
	label_array[1].ttl = 60;
	label_array[1].l3_intf_id = *ingress_intf;
	num_labels = 2;


    rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *tunnel_id = label_array[0].tunnel_id;
    return rv;
}

/* Main IPv4 MC Example with ports*/
int run_with_ports_ipv4_mc_example_flexible_table(int unit, int iport, int oport) {
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	/* Set parameters: User can set different parameters. */

	int ipmc_index = 6000;
	int dest_local_port_id = iport;
	int source_local_port_id = oport;
	int nof_dest_ports = 4, port_ndx;
	bcm_mac_t mac_l3_ingress = { 0x00, 0x00, 0x00, 0x00, 0xab, 0x1d };

	int grp;
	int entry;
	int default_entry;

	/*  the following elements are part of the key   */
	uint16 vrf = 0xabc;              /* part of the key */
	uint32 mask_vrf = 0xfff;         /* part of the mask */

	int vlan = 2;
	uint32 mask_in_rif = 0xfff;      /* part of the mask */

	bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
	uint32 mask_sip = 0x00000000;   /* part of the mask */

	bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
	uint32 mask_dip = 0x0fffffff;   /* part of the mask , mask_dip 4 msb must always be 0*/
	/* * * * * * * * * * * * * * * * * * * * * * * * */

	/* Create the IP MC Group */
	bcm_multicast_destroy(0, ipmc_index);
	bcm_multicast_t mc_id = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, ipmc_index, dest_local_port_id, nof_dest_ports, vlan, FALSE);

	/* Add member VLAN to each port in the multicast group */
	for (port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
		rv = bcm_vlan_gport_add(unit, vlan, dest_local_port_id + port_ndx, 0);
		if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
			printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
			return rv;
		}
	}

	/* Create ingress intf, for ethernet termination. */
    rv = vlan__open_vlan_per_mc(unit, vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, vlan, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, vlan);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = vlan;
    intf.my_global_mac = mac_l3_ingress;
    intf.my_lsb_mac = mac_l3_ingress;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

	/* Create the IP MC entry */

/* **  add ip_mc entries via field API (maskable with priority)        ** */

/* create an entry with priority 10*/
	rv = field_extended_ipv4_mc_group_create_if_not_exist(unit, &grp);
	if (BCM_E_NONE != rv) {
		printf("Error: in field_extended_ipv4_mc_group_create_if_not_exist\n");
		return rv;
	}


	rv = bcm_field_entry_create(unit, grp, &entry);
	if (BCM_E_NONE != rv) {
		printf("Error: in bcm_field_entry_create--\n");
		return rv;
	}

	/* add entry with wrong mc_id and wrong vlan*/
	rv = field_extended_ipv4_mc_entry_add(unit, grp
										  , vrf, mask_vrf /* totally masked vrf , in general ip_mc lookup we dont have vrf*/
										  , vlan + 1, mask_in_rif
										  , src_ip, mask_sip
										  , mc_ip, mask_dip
										  , mc_id - 1, entry, 11); /* wrong mc id will update it in next call*/
	if (BCM_E_NONE != rv) {
		printf("Error: in field_extended_ipv4_mc_entry_add\n");
		return rv;
	}
	/* update the entry give it the right mc_id*/
	rv = field_extended_ipv4_mc_entry_add(unit, grp
										  , vrf, mask_vrf /* totally masked vrf , in general ip_mc lookup we dont have vrf*/
										  , vlan, mask_in_rif
										  , src_ip, mask_sip
										  , mc_ip, mask_dip
										  , mc_id, entry, 10);
	if (BCM_E_NONE != rv) {
		printf("Error: in field_extended_ipv4_mc_entry_add\n");
		return rv;
	}


/* create a default entry*/

	rv = bcm_field_entry_create(unit, grp, &default_entry);
	if (BCM_E_NONE != rv) {
		printf("Error: in bcm_field_entry_create\n");
		return rv;
	}

	rv = field_extended_ipv4_mc_entry_add(unit, grp
										  , vrf, 0x0 /* totally masked vrf , in general ip_mc lookup we dont have vrf*/
										  , vlan, 0x0
										  , src_ip, 0x0
										  , mc_ip, 0x0
										  , 5000, default_entry, 9);
	if (BCM_E_NONE != rv) {
		printf("Error: in field_extended_ipv4_mc_entry_add\n");
		return rv;
	}


/* ******************************************************************* */

	return rv;
}


