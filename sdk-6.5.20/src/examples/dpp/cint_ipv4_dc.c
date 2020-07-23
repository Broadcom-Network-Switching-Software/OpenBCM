
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * Using the KBP device as external TCAM lookups (ELK), this CINT describes 
 * a configuration examples for IPv4 double capacity (2 million entries) . 
 *  
 *  
 *  
 * required SOC properties: 
 * ext_ip4_double_capacity_fwd_table_size=XXX 
 *  
 * number_of_inrif_mac_termination_combinations=8 (or less)
 * ext_tcam_result_size_segment_0=48
 * ext_tcam_result_size_segment_1=48
 * ext_tcam_result_size_segment_2=24
 * ext_tcam_result_size_segment_3= 16 to 64
 * ext_tcam_result_size_segment_pad_3=24 (indicates 24bit padding) 
 */



int ipv4_dc_verbose = 0;
bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};


/*
 * This cint is used to test the V4MC with IPMC disable case handling in per RIF configuration with the
 * BCM_L3_INGRESS_L3_MCAST_L2 flag and to make sure that the IPMC enable case is still working.
 *
 * Section 1: Create RIFs
 * Section 2: Creating MC groups
 * Section 3: Setting information relevant to MC forwarding, upon successful hits in LEM
 * Section 4: Defining entries for LEM which will forward the packet to the relevant MC group
*/

int ipv4_dc_intf_creation(int unit, int vrf){

    int i,rv = BCM_E_NONE;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;

    if (vrf > 255) {
		printf("VRF bigger than 255 is not supported\n");
        return -1;
	}

	bcm_l3_intf_t_init(&intf);

	sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
	intf.l3a_vrf = vrf;
	intf.l3a_vid = vrf;

	rv = bcm_l3_intf_create(unit, intf);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create");
        return -1;
	}

	bcm_l3_ingress_t_init(&ingress_intf);
	
	ingress_intf.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
	ingress_intf.vrf = vrf;

	rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create \n");
        return -1;
	}

    return rv;
}

int ipv4_dc_intf_creation_with_traffic(int unit, int base_vrf, int out_port, int num_of_hosts, int num_of_routes, int num_of_entries_in_lem, int is_double_capacity, int is_rpf){

    int i,rv = BCM_E_NONE;
    int vrf;
    bcm_l3_intf_t intf;
    bcm_l3_intf_t intf2;
    bcm_l3_intf_t intf3;
    bcm_l3_intf_t intf4;
	bcm_l3_ingress_t ingress_intf;
    create_l3_egress_s egress_intf;
    create_l3_egress_s egress_intf2;
    create_l3_egress_s egress_intf3;
	bcm_l3_host_t l3host;
	bcm_l3_route_t l3route;

    int host = 0x0a00ff01;
    int s_ip = 0xc0800101;
	bcm_mac_t next_hop_mac_host  = {0xb0, 0x0, 0x0, 0x0, 0x0, 0x54};
	bcm_mac_t next_hop_mac_route = {0xa0, 0x0, 0x0, 0x0, 0x0, 0x12};
	bcm_mac_t next_hop_mac_lem = {0xc0, 0x0, 0x0, 0x0, 0x0, 0x67};

	bcm_if_t eg_int;
	bcm_if_t eg_int2;
	bcm_if_t eg_int3;

    if (base_vrf > 55) {
		printf("VRF bigger than 55 is not supported in that test\n");
        return -1;
	}

    if ((is_double_capacity == 1) && (is_rpf == 1))
    {
      printf("ipv4 DC cannot be called with RPF indecation\n is_double_capacity=%d, is_rpf=%d\n",is_double_capacity,is_rpf);
      return -1;
    }

    vrf = base_vrf;

    /* 
     * Create Interface 1
     * Ingress Inteface
     * vrf = base_vrf 
 */ 
	bcm_l3_intf_t_init(&intf);
	sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
	intf.l3a_vrf = vrf;
	intf.l3a_vid = vrf;
	rv = bcm_l3_intf_create(unit, intf);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create\n");
        return -1;
	}

	bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;

    if(is_double_capacity == 1){
		ingress_intf.flags |= BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY;
	}

    if(is_rpf == 1){
		ingress_intf.urpf_mode = bcmL3IngressUrpfStrict;
	}

	ingress_intf.vrf = vrf;
	rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_ingress_create\n");
        return -1;
	}

    /* 
     * Create Interface 2
     * Egress Inteface for KPB hosts
     * vrf = 100+base_vrf 
 */ 
	bcm_l3_intf_t_init(&intf2);
	sal_memcpy(intf2.l3a_mac_addr, mac_address_1, 6);
	intf2.l3a_vrf = 100+vrf;
	intf2.l3a_vid = 100+vrf;
	rv = bcm_l3_intf_create(unit, intf2);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create 2\n");
        return -1;
	}

	egress_intf.next_hop_mac_addr = next_hop_mac_host;
    if (is_rpf == 1) {
        egress_intf.next_hop_mac_addr[5] += 1;
    }
	egress_intf.out_gport = out_port;
	egress_intf.out_tunnel_or_rif 	 = intf2.l3a_intf_id;
	rv = l3__egress__create(unit,&egress_intf);
	if (rv != BCM_E_NONE) {
		printf("Error, l3__egress__create 1\n");
        return -1;
	}
    eg_int = egress_intf.fec_id;

    /*
     * add hosts
     */ 
	bcm_l3_host_t_init(&l3host);
	l3host.l3a_ip_addr = host;
	l3host.l3a_intf = eg_int;
    l3host.l3a_vrf = vrf;
	if (is_double_capacity == 1) {
		l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE; /* add entry to DC */
	}

	for (i = 0; i < num_of_hosts; i++)
    {
		rv = bcm_l3_host_add(unit, &l3host);
		if (rv) {
			printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
			return rv;
		}
		l3host.l3a_ip_addr++;
    }

    if (is_rpf == 1)
    {
      l3host.l3a_ip_addr = s_ip;
      rv = bcm_l3_host_add(unit, &l3host);
      if (rv) {
          printf("Error, bcm_l3_host_add Failed on RPF, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
          return rv;
      }
    }

    /* 
     * Create Interface 3
     * Egress Inteface for KPB routs
     * vrf = 150+base_vrf 
 */ 
	bcm_l3_intf_t_init(&intf3);
	sal_memcpy(intf3.l3a_mac_addr, mac_address_1, 6);
	intf3.l3a_vrf = 150+vrf;
	intf3.l3a_vid = 150+vrf;
	rv = bcm_l3_intf_create(unit, intf3);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create 3\n");
        return -1;
	}

	egress_intf2.next_hop_mac_addr = next_hop_mac_route;
	egress_intf2.out_gport	  = out_port;
	egress_intf2.out_tunnel_or_rif 	  = intf3.l3a_intf_id;
	rv = l3__egress__create(unit,&egress_intf2);
	if (rv != BCM_E_NONE) {
		printf("Error, l3__egress__create 2\n");
        return -1;
	}
    eg_int2 = egress_intf2.fec_id;

    /*
     * add routes
     */ 
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = host;
    l3route.l3a_ip_mask = 0xFF800000;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = eg_int2;
	if (is_double_capacity == 1) {
		l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE; /* add entry to DC */
	}

	for (i = 0; i < num_of_routes; i++)
    {
		rv = bcm_l3_route_add(unit, &l3route);
		if (rv) {
			 printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
			 return rv;
		}
		l3route.l3a_subnet += 0x800000;
	}

    /* 
     * Create Interface 4
     * Egress Inteface for LEM hosts
     * vrf = 200+base_vrf 
     */ 

	bcm_l3_intf_t_init(&intf4);
	sal_memcpy(intf4.l3a_mac_addr, mac_address_1, 6);
	intf4.l3a_vrf = 200+vrf;
	intf4.l3a_vid = 200+vrf;
	rv = bcm_l3_intf_create(unit, intf4);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l3_intf_create 4\n");
        return -1;
	}

	egress_intf3.next_hop_mac_addr = next_hop_mac_lem;
	egress_intf3.out_gport	  = out_port;
	egress_intf3.out_tunnel_or_rif 	  = intf4.l3a_intf_id;
	rv = l3__egress__create(unit,&egress_intf3);
	if (rv != BCM_E_NONE) {
		printf("Error, l3__egress__create 3\n");
        return -1;
	}
    eg_int3 = egress_intf3.fec_id;

    /*
     * add lem hosts
     */ 
	bcm_l3_host_t_init(&l3host);
	l3host.l3a_ip_addr = host;
	l3host.l3a_intf = eg_int3;
    l3host.l3a_vrf = vrf;
	if (is_double_capacity == 1) {
		l3host.l3a_flags = BCM_L3_HOST_LOCAL;
	}

	for (i = 0; i < num_of_entries_in_lem; i++)
    {
		rv = bcm_l3_host_add(unit, &l3host);
		if (rv) {
			printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
			return rv;
		}
		l3host.l3a_ip_addr++;
    }
	
    return rv;
}

int ipv4_dc_semantic_max_entries_insertion(int unit, int vrf, int num_of_entries){

    int i, rv = 0;
    int entry_num_print_mod = 0;

    int fec = 0x20000400;
    int encap_id = 0x40001000;
    int host = 0x0a00ff01;

    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l3_host_t l3host;
	bcm_l3_route_t l3route;


    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
	l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE; /* add entry to DC */
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = 0; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;

    l3host.l3a_port_tgid = 201; /* egress port to send packet to */
    l3host.encap_id = encap_id;
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags = 0;
	l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE; /* add entry to DC */
    l3route.l3a_subnet = host;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = 0; /* point to FEC to get out-interface  */
    l3route.l3a_modid = 0;
    l3route.l3a_port_tgid = 0;

    printf("stage 1, adding hosts and routes to external tcam\n");
    for (i = 0; i < num_of_entries; i ++)
    {        
		rv = bcm_l3_host_add(unit, &l3host);
				
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);

        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }
        
        if ((i % 1000) == 1) {
            printf("time=%u[us] entry_id=%d, l3host.l3a_ip_addr=0x%x\n", sal_time_usecs(), i, l3host.l3a_ip_addr);
        }

        l3host.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }

	printf("stage 1 finished, %d hosts and %d routes was added to external tcam\n", num_of_entries, num_of_entries);

    printf("stage 2, deleting hosts and routes from the external tcam\n");

	l3host.l3a_ip_addr = host;
    l3route.l3a_subnet = host;	
	for (i = 0; i < num_of_entries; i ++)
    {        
		rv = bcm_l3_host_delete(unit, &l3host);
				
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);

        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

        l3host.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }
	printf("stage 2 finished, %d hosts and %d routes removed from the external tcam\n", num_of_entries, num_of_entries);    

	return rv;
}

int cint_ipv4_dc_lem_scale_semantic_4k_entries_iterations(int unit,int num_of_iterations){

    int i,j,rv = BCM_E_NONE;
	bcm_l3_host_t l3host,l3host_lem;
	bcm_l3_route_t l3route;
	bcm_l3_host_t host_info;
	bcm_l3_host_t host_info_lem;
	bcm_l3_route_t route_info;

	int route_mask = 0xFFFFE000;
	int route_add = 0x2000;
	int route_base = 0x0a002001;

	int host = 0x0a000001;

	int vrf = 17;

	int fec = 0x20001000;

	for (j=0;j<num_of_iterations;j++) {
		fec = 0x20001000 + (j<<12);

		printf("adding 4K hosts, routs, hosts in lem. iteration%d\n",j);
		bcm_l3_host_t_init(&l3host);
		l3host.l3a_ip_addr = host;
		l3host.l3a_intf = fec;
		l3host.l3a_vrf = vrf;
		l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

		bcm_l3_route_t_init(&l3route);
		l3route.l3a_subnet = route_base;
		l3route.l3a_ip_mask = route_mask;
		l3route.l3a_vrf = vrf;
		l3route.l3a_intf = fec;
		l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

		bcm_l3_host_t_init(&l3host_lem);
		l3host_lem.l3a_ip_addr = host;
		l3host_lem.l3a_intf = fec;
		l3host_lem.l3a_vrf = vrf;
		l3host_lem.l3a_flags = BCM_L3_HOST_LOCAL;

		for (i = 0; i < 4096; i++)
		{
			rv = bcm_l3_host_add(unit, &l3host);
			if (rv) {
				printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
				return rv;
			}
			rv = bcm_l3_route_add(unit, &l3route);
			if (rv) {
				 printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
				 return rv;
			}
			rv = bcm_l3_host_add(unit, &l3host_lem);
			if (rv) {
				printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
				return rv;
			}
			l3host.l3a_ip_addr++;
			l3route.l3a_subnet += route_add;
			l3host_lem.l3a_ip_addr++;
			l3host.l3a_intf++;
			l3route.l3a_intf++;
			l3host_lem.l3a_intf++;
		}

		printf("get 4K hosts and routes. if exists, delete it.\n");
		host_info.l3a_ip_addr = host;
		host_info.l3a_vrf = vrf;
		host_info.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

		route_info.l3a_ip_mask = route_mask;
		route_info.l3a_subnet = route_base;
		route_info.l3a_vrf = vrf;
		route_info.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

		host_info_lem.l3a_ip_addr = host;
		host_info_lem.l3a_vrf = vrf;
		host_info_lem.l3a_flags = BCM_L3_HOST_LOCAL;

		for (i=0;i<4096;i++) {

			rv = bcm_l3_host_find(unit,&host_info);
			if (rv) {
				 printf("Error, bcm_l3_host_find Failed, entry=%d, l3a_ip_addr=0x%x, rv=0x%x\n", i, host_info.l3a_ip_addr, rv);
				 return rv;
			}
			if (host_info.l3a_intf != (fec + i)) {
				printf("Error in returned host fec value: index=%d, expected=0x%08x actual=0x%08x\n",i, fec_host + i, host_info.l3a_intf);
				return -1;
			}
			rv = bcm_l3_host_delete(unit,&host_info);
			if (rv) {
				 printf("Error, bcm_l3_host_delete Failed, index=%d, rv=0x%x\n", i, rv);
				 return rv;
			}

			rv = bcm_l3_route_get(unit,&route_info);
			if (rv) {
				 printf("Error, bcm_l3_route_get Failed, entry=%d, l3a_subnet=0x%x, rv=0x%x\n", i, route_info.l3a_subnet, rv);
				 return rv;
			}
			if (route_info.l3a_intf != (fec + i)) {
				printf("Error in returned route fec value: index=%d, expected=0x%08x actual=0x%08x\n",i, fec_route + i, route_info.l3a_intf);
				return -1;
			}
			rv = bcm_l3_route_delete(unit,&route_info);
			if (rv) {
				 printf("Error, bcm_l3_route_delete Failed, index=%d, rv=0x%x\n", i, rv);
				 return rv;
			}

			rv = bcm_l3_host_find(unit,&host_info_lem);
			if (rv) {
				 printf("Error, bcm_l3_host_find Failed, entry=%d, l3a_ip_addr=0x%x, rv=0x%x\n", i, host_info_lem.l3a_ip_addr, rv);
				 return rv;
			}
			if (host_info_lem.l3a_intf != (fec + i)) {
				printf("Error in returned host fec value: index=%d, expected=0x%08x actual=0x%08x\n",i, fec_host + i, host_info_lem.l3a_intf);
				return -1;
			}
			rv = bcm_l3_host_delete(unit,&host_info_lem);
			if (rv) {
				 printf("Error, bcm_l3_host_delete Failed, index=%d, rv=0x%x\n", i, rv);
				 return rv;
			}

			route_info.l3a_subnet += route_add;
			host_info.l3a_ip_addr++;
			host_info_lem.l3a_ip_addr++;
		}
	}

	printf("adding 1 entry to each DB for the traffic test\n");
	bcm_l3_host_t_init(&l3host);
	l3host.l3a_ip_addr = host;
	l3host.l3a_intf = 0x20001000;
	l3host.l3a_vrf = vrf;
	l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

	bcm_l3_route_t_init(&l3route);
	l3route.l3a_subnet = route_base;
	l3route.l3a_ip_mask = route_mask;
	l3route.l3a_vrf = vrf;
	l3route.l3a_intf = 0x20001001;
	l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;

	bcm_l3_host_t_init(&l3host_lem);
	l3host_lem.l3a_ip_addr = host;
	l3host_lem.l3a_intf = 0x20001002;
	l3host_lem.l3a_vrf = vrf;
	l3host_lem.l3a_flags = BCM_L3_HOST_LOCAL;

	rv = bcm_l3_host_add(unit, &l3host);
	if (rv) {
		printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
		return rv;
	}
	rv = bcm_l3_route_add(unit, &l3route);
	if (rv) {
		 printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
		 return rv;
	}
	rv = bcm_l3_host_add(unit, &l3host_lem);
	if (rv) {
		printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%08x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
		return rv;
	}

	return rv;
}

int ipv4_dc_drop_by_pmf(int unit, int hitbit){
	int rv = 0;

	rv = configure_pmf_action_by_hit_bit_example(unit,hitbit,bcmFieldAppTypeIp4DoubleCapacity, bcmFieldActionDrop, 0, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit %d error\n",hitbit);
		return  rv;
	}
	return rv;
}

int ipv4_dc_drop_by_pmf_by_value(int unit, int hitbit, uint32 val0, uint32 val1, uint32 mask0, uint32 mask1, int priority){
	int rv = 0;
	uint64 val,mask;

	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);
	COMPILER_64_SET(val ,val1,val0);
	COMPILER_64_SET(mask,mask1,mask0);
	rv = configure_pmf_action_by_hit_value_example(unit, hitbit, val, mask, bcmFieldAppTypeIp4DoubleCapacity, 0, bcmFieldActionDrop, 0, 0, priority, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_value_example bit %d error\n",hitbit);
		return  rv;
	}
	return rv;
}

int host_check_list[4096] = {0};
int route_check_list[4096] = {0};
int host_lem_check_list[4096] = {0};

int host_counter = 0;
int route_counter = 0;

int ipv4_kbp_host_traverse_cb(int unit, int index, bcm_l3_host_t *info, void *is_dc){

	uint32 base_address;
	uint32 expected_fec;
	uint32 expected_encap;
	uint32 expected_flags2;
	int *check_list;
	int i;
    uint8 *is_dc_ptr = is_dc;
    
    if (*is_dc_ptr == 0) {
        expected_flags2 = 0;
    } else{
        expected_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    }

	if (index < 4096) {
		base_address = 0x0c000000;
		check_list = host_lem_check_list;
		expected_fec = 0x20001002;
		expected_encap = 0x40001001;
		if(!(info->l3a_flags & BCM_L3_HOST_LOCAL)){
			printf("Error: expected to have BCM_L3_HOST_LOCAL set. flags = 0x%08x\n",info->l3a_flags);
			return BCM_E_FAIL;
		}
	}else{
		base_address = 0x0a000000;
		check_list = host_check_list;
		expected_fec = 0x20001000;
		expected_encap = 0x40001000;
		if(info->l3a_flags2 != expected_flags2){
			printf("Error: expected to have BCM_L3_FLAGS2_SCALE_ROUTE set. flags2 = 0x%08x, expected = 0x%08x\n",info->l3a_flags2,expected_flags2);
			return BCM_E_FAIL;
		}
	}
	if ((info->l3a_ip_addr&0xFF000000)!=base_address) {
		printf("Error: index=%d, ip_address=0x%08x,  expected=0x%08x\n",index,info->l3a_ip_addr,base_address);
		return BCM_E_FAIL;
	}
	if (check_list[info->l3a_ip_addr&0x0FFF] == 1) {
		printf("Error: index=%d, ip_address=0x%08x, 2nd check_list hit on cell=%d:\n",index,info->l3a_ip_addr,info->l3a_ip_addr&0x1000);
		return BCM_E_FAIL;
	}else{
		check_list[info->l3a_ip_addr&0x0FFF] = 1;
	}
	if (info->l3a_vrf != ((info->l3a_ip_addr&0x0000FFF)>>8)) {
		printf("Error: index=%d, ip_address=0x%08x, vrf=%d. VRF matching error, expected=%d\n",index,info->l3a_ip_addr,(info->l3a_ip_addr&0x0000FFF)>>8);
		return BCM_E_FAIL;				
	}
	if (info->l3a_intf != expected_fec) {
		printf("Error: index=%d, ip_address=0x%08x, info->l3a_intf=0x%08x. fec matching error, expected=0x%08x\n",index,info->l3a_ip_addr,info->l3a_intf, expected_fec);
		return BCM_E_FAIL;				
	}
	if (info->encap_id != expected_encap) {
		printf("Error: index=%d, ip_address=0x%08x, info->encap_id=0x%08x. encap_id matching error, expected=0x%08x\n",index,info->l3a_ip_addr,info->encap_id, expected_encap);
		return BCM_E_FAIL;				
	}

	host_counter++;
	if ((host_counter&0xFF) == 0) {
		printf("Host traverse index: %d verified\n", index);
	}

	if (index == 8191) {
		printf("TRAVERSED ALL HOSTS!!\n");
		printf("check the hosts checking list:\n");
		for (i=0;i<4096;i++) {
			if (host_check_list[i] != 1) {
				printf("Error: host_check_list[%d]=%d\n",i,host_check_list[i]);
				return BCM_E_FAIL;
			}
			if (host_lem_check_list[i] != 1) {
				printf("Error: host_lem_check_list[%d]=%d\n",i,host_lem_check_list[i]);
				return BCM_E_FAIL;
			}
		}
		printf("verified host checking lists\n");
	}
	return 0;
}

int ipv4_kbp_route_traverse_cb(int unit, int index, bcm_l3_route_t *info, void* is_dc){

	uint32 base_address;
	uint32 expected_fec;
    uint32 expected_flags2;
	int *check_list;
	int i;
    uint8 *is_dc_ptr = is_dc;

	base_address = 0x0b000000;
	check_list = route_check_list;
	expected_fec = 0x20001001;


    if (*is_dc_ptr == 0) {
        expected_flags2 = 0;
    } else{
        expected_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    }

    if(info->l3a_flags2 != expected_flags2){
        printf("Error: expected to have BCM_L3_FLAGS2_SCALE_ROUTE set. flags2 = 0x%08x, expected = 0x%08x\n",info->l3a_flags2,expected_flags2);
		return BCM_E_FAIL;
	}

	if ((info->l3a_subnet&0xFF000000)!=base_address) {
		printf("Error: index=%d, ip_address=0x%08x, expected=0x%08x\n",index,info->l3a_subnet,base_address);
		return BCM_E_FAIL;
	}
	if (check_list[(info->l3a_subnet&0x000FFF00)>>8] == 1) {
		printf("Error: index=%d, ip_address=0x%08x, 2nd check_list hit on cell=%d:\n",index,info->l3a_subnet,(info->l3a_subnet&0x000FFF00)>>8);
		return BCM_E_FAIL;
	}else{
		check_list[(info->l3a_subnet&0x000FFF00)>>8] = 1;
	}
	if (info->l3a_vrf != ((info->l3a_subnet&0x000FFF00)>>16)) {
		printf("Error: index=%d, ip_address=0x%08x, vrf=%d. VRF matching error, expected=%d\n",index,info->l3a_subnet,info->l3a_vrf,(info->l3a_subnet&0x000FFF00)>>16);
		return BCM_E_FAIL;				
	}
	if (info->l3a_intf != expected_fec) {
		printf("Error: index=%d, ip_address=0x%08x, info->l3a_intf=0x%08x. fec matching error, expected=0x%08x\n",index,info->l3a_subnet,info->l3a_intf, expected_fec);
		return BCM_E_FAIL;				
	}

	route_counter++;
	if ((route_counter&0xFF) == 0) {
		printf ("Route traverse index: %d verified\n", index);
	}

	if (index == 4095) {
		printf("TRAVERSED ALL ROUTES!!\n");
		printf("check the route checking list\n");
		for (i=0;i<4096;i++) {
			if (route_check_list[i] != 1) {
				printf("Error: route_check_list[%d]=%d\n",i,route_check_list[i]);
				return BCM_E_FAIL;
			}
		}
		printf("verified route checking list\n");
	}
	return 0;
}

int ipv4_kbp_traverse(int unit, uint8 is_dc){

    int i, rv = 0;

	int vrf = 0;

	uint32 fec_host = 0x20001000;
	uint32 fec_route = 0x20001001;
	uint32 fec_host_lem = 0x20001002;

	uint32 encap_id = 0x40001000;
	uint32 encap_id_lem = 0x40001001;

    uint32 host  = 0x0a000000;
	uint32 route = 0x0b000000;
	uint32 host_lem = 0x0c000000;

    uint32 flags2;
    
    bcm_l3_host_t  l3host;
	bcm_l3_route_t l3route;
    bcm_l3_host_t  l3host_lem;

    if (is_dc == 0) {
        flags2 = 0;
    }else{
        flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    }

    /*STEP 1: 4K hosts, 4K route, 4K lems*/
    bcm_l3_host_t_init(&l3host);
	l3host.l3a_flags2 = flags2;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec_host;
    l3host.encap_id = encap_id;

    bcm_l3_route_t_init(&l3route);
	l3route.l3a_flags2 = flags2; /* add entry to DC */
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;

    bcm_l3_host_t_init(&l3host_lem);
	l3host_lem.l3a_flags = BCM_L3_HOST_LOCAL; /* add entry to LEM */
    l3host_lem.l3a_ip_addr = host_lem;
    l3host_lem.l3a_vrf = vrf;
    l3host_lem.l3a_intf = fec_host_lem;
    l3host_lem.encap_id = encap_id_lem;

    for (i = 1; i <= 4096; i++)
    {        
		rv = bcm_l3_host_add(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

		rv = bcm_l3_host_add(unit, &l3host_lem);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host_lem.l3a_ip_addr, rv);
             return rv;
        }

        if ((i & 0xFF) == 0) {
			l3host.l3a_vrf++;
			l3route.l3a_vrf++;
			l3host_lem.l3a_vrf++;
		}

        l3host.l3a_ip_addr++;
		l3host_lem.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }

	rv = bcm_l3_host_traverse(unit,0,0,0,ipv4_kbp_host_traverse_cb, &is_dc);
	if (rv != BCM_E_NONE) {
		 printf("Error, bcm_l3_host_traverse \n");
		 return rv;
	}
	if (host_counter != 8192) {
		 printf("Error, host_counter = %d, expected 8192 \n",host_counter);
		 return -1;
	}
	rv = bcm_l3_route_traverse(unit, 0, 0, 0, ipv4_kbp_route_traverse_cb, &is_dc);
	if (rv != BCM_E_NONE) {
		 printf("Error, bcm_l3_route_traverse \n");
		 return rv;
	}
	if (route_counter != 4096) {
		 printf("Error, route_counter = %d, expected 8192 \n",route_counter);
		 return -1;
	}

	bcm_l3_host_t_init(&l3host);
	l3host.l3a_flags2 = flags2;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec_host;
    l3host.encap_id = encap_id;

    bcm_l3_route_t_init(&l3route);
	l3route.l3a_flags2 = flags2;
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;

    bcm_l3_host_t_init(&l3host_lem);
	l3host_lem.l3a_flags = BCM_L3_HOST_LOCAL; /* add entry to LEM */
    l3host_lem.l3a_ip_addr = host_lem;
    l3host_lem.l3a_vrf = vrf;
    l3host_lem.l3a_intf = fec_host_lem;
    l3host_lem.encap_id = encap_id_lem;

    for (i = 1; i <= 4096; i++)
    {        
		rv = bcm_l3_host_delete(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

		rv = bcm_l3_host_delete(unit, &l3host_lem);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host_lem.l3a_ip_addr, rv);
             return rv;
        }

        if ((i & 0xFF) == 0) {
			l3host.l3a_vrf++;
			l3route.l3a_vrf++;
			l3host_lem.l3a_vrf++;
		}

        l3host.l3a_ip_addr++;
		l3host_lem.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }

	/*STEP 2: 4K hosts, 2K hosts as routes , 2K routes*/
	return rv;
}

int ipv4_kbp_semantic(int unit){

    int i, rv = 0;
	int vrf = 0;

	uint32 fec_host = 0x20001000;
	uint32 fec_route = 0x20001001;

	uint32 encap_id = 0x40001000;

    uint32 host  = 0x0a000000;
	uint32 route = 0x0b000000;

    bcm_l3_host_t  l3host;
	bcm_l3_route_t l3route;

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec_host;
    l3host.encap_id = encap_id;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;

    printf("entries add - start\n");

    for (i = 1; i <= 16; i++)
    {        
		rv = bcm_l3_host_add(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

		l3host.l3a_vrf++;
		l3route.l3a_vrf+=4;
        l3host.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }
    printf("entries add - end\n");

	bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;

    printf("entries get - start\n");
    for (i = 1; i <= 16; i++)
    {        
		rv = bcm_l3_host_find(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_find Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }
        if (l3host.encap_id != encap_id) {
             printf("Error, host: encap_id Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip_addr=0x%x\n",encap_id,l3host.encap_id, i, l3host.l3a_ip_addr);
             return rv;
        }
        if (l3host.l3a_intf != fec_host) {
             printf("Error, host: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_ip_addr=0x%x\n",fec_host,l3host.l3a_intf, i, l3host.l3a_ip_addr);
             return rv;
        }

        rv = bcm_l3_route_get(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_get Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }
        if (l3route.l3a_intf != fec_route) {
             printf("Error, route: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_subnet=0x%x\n",fec_route,l3route.l3a_intf, i, l3route.l3a_subnet);
             return rv;
        }
        printf("Host - Entry %d: ip = 0x%08x, vrf = %d, fec=0x%08x, encap_id=0x%08x\n",i,l3host.l3a_ip_addr,l3host.l3a_vrf,l3host.l3a_intf,l3host.encap_id);
        printf("Route- Entry %d: ip = 0x%08x, mask=0x%08x, vrf = %d, fec=0x%08x\n",i,l3route.l3a_subnet,l3route.l3a_ip_mask,l3route.l3a_vrf,l3route.l3a_intf);
		l3host.l3a_vrf++;
		l3route.l3a_vrf+=4;
        l3host.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
        l3host.l3a_intf = 0;
        l3host.encap_id = 0;
        l3route.l3a_intf = 0;
    }
    printf("entries get - end\n");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = vrf;

    printf("entries delete - start\n");
    for (i = 1; i <= 16; i++)
    {        
		rv = bcm_l3_host_delete(unit, &l3host);
		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_delete Failed, entry %d, l3a_ip_addr=0x%x, rv=0x%x\n", i, l3host.l3a_ip_addr, rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

		l3host.l3a_vrf++;
		l3route.l3a_vrf+=4;
        l3host.l3a_ip_addr++;
		l3route.l3a_subnet+=0x100;
    }
    printf("entries delete - end\n");
    return 0;
}


int acl_ipv4dc_2nd_lookup_hit_intf_creation_with_traffic(int unit, int out_port, int is_raw) {

    int i, rv = 0;
    int vrf = 100;
    int vrf_host = vrf + 1;
    int vrf_route = vrf + 2;
    bcm_l3_intf_t intf;
    bcm_l3_intf_t intf_host;
    bcm_l3_intf_t intf_route;
    bcm_l3_ingress_t ingress_intf;
    create_l3_egress_s egress_intf_host;
    create_l3_egress_s egress_intf_route;
    bcm_l3_route_t l3host;
    bcm_l3_route_t l3route;

    int host = 0x0a000100;
    int route = 0x0b00ff01;
    int mask = 0xffffff00;
    bcm_mac_t next_hop_mac  = { 0x11, 0x0, 0x0, 0x0, 0x0, 0x11 };

    bcm_if_t eg_int_host;
    bcm_if_t eg_int_route;
    int fec_host = 0x1234;
    int fec_route = 0x4321;
    int flags2 = is_raw ? BCM_L3_FLAGS2_RAW_ENTRY : 0;

    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
    intf.l3a_vrf = vrf;
    intf.l3a_vid = vrf;
    rv = bcm_l3_intf_create(unit, intf);
    if(rv) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags =
                        BCM_L3_INGRESS_WITH_ID |
                        BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    ingress_intf.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if(rv) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    bcm_l3_intf_t_init(&intf_host);
    sal_memcpy(intf_host.l3a_mac_addr, mac_address_1, 6);
    intf_host.l3a_vrf = vrf_host;
    intf_host.l3a_vid = vrf_host;
    rv = bcm_l3_intf_create(unit, &intf_host);
    if(rv) {
        printf("Error, bcm_l3_intf_create host\n");
        return rv;
    }

    egress_intf_host.allocation_flags = BCM_L3_WITH_ID;
    egress_intf_host.fec_id = fec_host;
    egress_intf_host.next_hop_mac_addr = next_hop_mac;
    egress_intf_host.out_gport = out_port;
    egress_intf_host.out_tunnel_or_rif = intf_host.l3a_intf_id;
    rv = l3__egress__create(unit, &egress_intf_host);
    if(rv) {
        printf("Error, l3__egress__create host\n");
        return rv;
    }
    eg_int_host = egress_intf_host.fec_id;

    printf("\nAdding 4096 host entries and 1 route entry, expecting it to be in the second part of the IPv4 DC table\n\n");

    for(i = 0; i < 4096; i++) {
        bcm_l3_route_t_init(&l3host);
        l3host.l3a_subnet = host;
        l3host.l3a_ip_mask = mask;
        l3host.l3a_intf = fec_host;
        l3host.l3a_vrf = vrf;
        l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE | flags2; /* add entry to DC */
        rv = bcm_l3_route_add(unit, &l3host);
        if(rv) {
            printf("Error, bcm_l3_route_add Failed, l3host.l3a_subnet=0x%08x, rv=0x%x\n", l3host.l3a_subnet, rv);
            return rv;
        }
        host += 0x100;
    }

    bcm_l3_intf_t_init(&intf_route);
    sal_memcpy(intf_route.l3a_mac_addr, mac_address_1, 6);
    intf_route.l3a_vrf = vrf_route;
    intf_route.l3a_vid = vrf_route;
    rv = bcm_l3_intf_create(unit, &intf_route);
    if(rv) {
        printf("Error, bcm_l3_intf_create route\n");
        return rv;
    }

    egress_intf_route.allocation_flags = BCM_L3_WITH_ID;
    egress_intf_route.fec_id = fec_route;
    egress_intf_route.next_hop_mac_addr = next_hop_mac;
    egress_intf_route.out_gport = out_port;
    egress_intf_route.out_tunnel_or_rif = intf_route.l3a_intf_id;
    rv = l3__egress__create(unit, &egress_intf_route);
    if(rv) {
        printf("Error, l3__egress__create route\n");
        return rv;
    }
    eg_int_route = egress_intf_route.fec_id;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = mask;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = fec_route;
    l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE | flags2; /* add entry to DC */

    rv = bcm_l3_route_add(unit, &l3route);
    if(rv) {
         printf("Error, bcm_l3_route_add Failed, l3a_subnet=0x%x, rv=0x%x\n", l3route.l3a_subnet, rv);
         return rv;
    }

    return rv;
}

int raw_entries_dc_sem(int unit) {

    int i, rv = BCM_E_NONE;
    int addr_ip = 0x12345678;
    int mask_ip = 0xffffffff;
    int half_ip = 0xffff0000;
    int zero_ip = 0x00000000;

    bcm_if_t base_fec = 0x12345;
    bcm_if_t max_fec = 0xfffff;
    bcm_if_t out_of_range_fec = 0x100000;
    int base_vrf = 0x64;

    bcm_l3_host_t    host;
    bcm_l3_route_t   route;

    printf("IPv4 DC route entries\n");

    printf("Part 1 - default route (KAPS)\n");
    bcm_l3_route_t_init(&route);
    route.l3a_flags2 = BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_SCALE_ROUTE;
    route.l3a_subnet = zero_ip;
    route.l3a_ip_mask = zero_ip;
    route.l3a_vrf = base_vrf;
    route.l3a_intf = base_fec;

    printf("Testing regular FEC\n");
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_add failed with %d\n", rv);
        return rv;
    }
    printf("  added\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_get failed with %d\n", rv);
        return rv;
    }
    printf("  found\n");
    if(route.l3a_intf != base_fec) {
        printf("Verification failed; fec_id found %X, expected %X\n", route.l3a_intf, base_fec);
        return BCM_E_FAIL;
    }
    printf("  verified\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_delete with %d\n", rv);
        return rv;
    }
    printf("  removed\n");

    printf("Testing maximum-sized FEC\n");
    route.l3a_intf = max_fec;
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_add failed with %d\n", rv);
        return rv;
    }
    printf("  added\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_get failed with %d\n", rv);
        return rv;
    }
    printf("  found\n");
    if(route.l3a_intf != max_fec) {
        printf("Verification failed; fec_id found %X, expected %X\n", route.l3a_intf, max_fec);
        return BCM_E_FAIL;
    }
    printf("  verified\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_delete with %d\n", rv);
        return rv;
    }
    printf("  removed\n");

    printf("Testing out of range FEC\n");
    route.l3a_intf = out_of_range_fec;
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_INTERNAL) {
        printf("bcm_l3_route_add failed with %d, expected %d\n", rv, BCM_E_INTERNAL);
        return BCM_E_INTERNAL;
    }
    printf("  add expected internal failure\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NOT_FOUND) {
        printf("bcm_l3_route_get failed with %d, expected %d\n", rv, BCM_E_NOT_FOUND);
        return BCM_E_NOT_FOUND;
    }
    printf("  get expected not found failure\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NOT_FOUND) {
        printf("bcm_l3_route_delete with %d, expected %d\n", rv, BCM_E_NOT_FOUND);
        return BCM_E_NOT_FOUND;
    }
    printf("  delete expected not found failure\n");

    printf("Part 2 - regular route (KBP)\n");
    bcm_l3_route_t_init(&route);
    route.l3a_flags2 = BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_SCALE_ROUTE;
    route.l3a_subnet = addr_ip;
    route.l3a_ip_mask = half_ip;
    route.l3a_vrf = base_vrf;
    route.l3a_intf = base_fec;

    printf("Testing regular FEC\n");
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_add failed with %d\n", rv);
        return rv;
    }
    printf("  added\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_get failed with %d\n", rv);
        return rv;
    }
    printf("  found\n");
    if(route.l3a_intf != base_fec) {
        printf("Verification failed; fec_id found %X, expected %X\n", route.l3a_intf, base_fec);
        return BCM_E_FAIL;
    }
    printf("  verified\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_delete with %d\n", rv);
        return rv;
    }
    printf("  removed\n");

    printf("Testing maximum-sized FEC\n");
    route.l3a_intf = max_fec;
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_add failed with %d\n", rv);
        return rv;
    }
    printf("  added\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_get failed with %d\n", rv);
        return rv;
    }
    printf("  found\n");
    if(route.l3a_intf != max_fec) {
        printf("Verification failed; fec_id found %X, expected %X\n", route.l3a_intf, max_fec);
        return BCM_E_FAIL;
    }
    printf("  verified\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_route_delete with %d\n", rv);
        return rv;
    }
    printf("  removed\n");

    printf("Testing out of range FEC\n");
    route.l3a_intf = out_of_range_fec;
    rv = bcm_l3_route_add(unit, &route);
    if(rv != BCM_E_INTERNAL) {
        printf("bcm_l3_route_add failed with %d, expected %d\n", rv, BCM_E_INTERNAL);
        return BCM_E_INTERNAL;
    }
    printf("  add expected internal failure\n");
    route.l3a_intf = 0;

    rv = bcm_l3_route_get(unit, &route);
    if(rv != BCM_E_NOT_FOUND) {
        printf("bcm_l3_route_get failed with %d, expected %d\n", rv, BCM_E_NOT_FOUND);
        return BCM_E_NOT_FOUND;
    }
    printf("  get expected not found failure\n");
    rv = bcm_l3_route_delete(unit, &route);
    if(rv != BCM_E_NOT_FOUND) {
        printf("bcm_l3_route_delete with %d, expected %d\n", rv, BCM_E_NOT_FOUND);
        return BCM_E_NOT_FOUND;
    }
    printf("  delete expected not found failure\n");

    return BCM_E_NONE;
}

int default_route_dc_config_example(int unit, int out_port) {

    int rv = 0;
    int vlan = 100;
    int vrf = 100;
    bcm_l3_intf_t intf;
    bcm_l3_intf_t intf2;
    bcm_l3_intf_t intf_host;
    bcm_l3_intf_t intf_host2;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_ingress_t ingress_intf2;
    bcm_l3_egress_t egress_intf;
    bcm_if_t eg_int1;
    bcm_if_t eg_int2;
    bcm_l3_host_t l3host;
    bcm_l3_route_t l3route;
    int ip_hit1 = 0x0a000001;
    int ip_zero = 0x00000000;
    int ip_mask = 0xffffffff;

    bcm_mac_t my_mac = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x00 };
    bcm_mac_t my_mac2 = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x0c };
    bcm_mac_t next_hop_mac = { 0x11, 0x00, 0x00, 0x00, 0x00, 0x11 };
    bcm_mac_t next_hop_mac2 = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };

    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac, 6);
    intf.l3a_vrf = vrf;
    intf.l3a_vid = vlan;
    rv = bcm_l3_intf_create(unit, &intf);
    if(rv) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags =
                        BCM_L3_INGRESS_WITH_ID |
                        BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    ingress_intf.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if(rv) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }



    bcm_l3_intf_t_init(&intf2);
    sal_memcpy(intf2.l3a_mac_addr, my_mac2, 6);
    intf2.l3a_vrf = vrf + 2;
    intf2.l3a_vid = vlan + 2;
    rv = bcm_l3_intf_create(unit, &intf2);
    if(rv) {
        printf("Error, bcm_l3_intf_create2\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf2);
    ingress_intf2.flags =
                        BCM_L3_INGRESS_WITH_ID |
                        BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST |
                        BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    ingress_intf2.vrf = vrf + 2;
    rv = bcm_l3_ingress_create(unit, &ingress_intf2, &intf2.l3a_intf_id);
    if(rv) {
        printf("Error, bcm_l3_ingress_create2\n");
        return rv;
    }

    bcm_l3_intf_t_init(&intf_host);
    sal_memcpy(intf_host.l3a_mac_addr, my_mac, 6);
    intf_host.l3a_vrf = vrf;
    intf_host.l3a_vid = vlan + 1;
    rv = bcm_l3_intf_create(unit, &intf_host);
    if(rv) {
        printf("Error, bcm_l3_intf_create host\n");
        return rv;
    }
    bcm_l3_intf_t_init(&intf_host2);
    sal_memcpy(intf_host2.l3a_mac_addr, my_mac2, 6);
    intf_host2.l3a_vrf = vrf + 2;
    intf_host2.l3a_vid = vlan + 3;
    rv = bcm_l3_intf_create(unit, &intf_host2);
    if(rv) {
        printf("Error, bcm_l3_intf_create host2\n");
        return rv;
    }

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.port = out_port;
    sal_memcpy(egress_intf.mac_addr, next_hop_mac, 6);
    egress_intf.intf = intf_host.l3a_intf_id;
    egress_intf.vlan = vlan;
    rv = bcm_l3_egress_create(unit, 0, &egress_intf, &eg_int1);
    if(rv) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.port = out_port;
    sal_memcpy(egress_intf.mac_addr, next_hop_mac2, 6);
    egress_intf.intf = intf_host2.l3a_intf_id;
    egress_intf.vlan = vlan;
    rv = bcm_l3_egress_create(unit, 0, &egress_intf, &eg_int2);
    if(rv) {
        printf("Error, bcm_l3_egress_create2\n");
        return rv;
    }

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = ip_hit1;
    l3route.l3a_ip_mask = ip_mask;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = eg_int1;
    l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    rv = bcm_l3_route_add(unit, &l3route);
    if(rv) {
        printf("Error, bcm_l3_route_add Failed, l3a_subnet=0x%x\n", l3route.l3a_subnet);
        return rv;
    }
    l3route.l3a_subnet = ip_zero;
    l3route.l3a_ip_mask = ip_zero;
    l3route.l3a_intf = eg_int2;
    l3route.l3a_vrf = vrf+2;
    rv = bcm_l3_route_add(unit, &l3route);
    if(rv) {
        printf("Error, bcm_l3_route_add Failed, l3a_subnet=0x%x\n", l3route.l3a_subnet);
        return rv;
    }

    return 0;
}

int default_route_dc_remove_entry(int unit, int dr) {

    int rv = 0;
    int ip = dr ? 0x0 : 0x0a000001;
    int mask = dr ? 0x0 : 0xffffffff;
    int vrf = dr ? 102 : 100;
    bcm_l3_route_t l3route;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    l3route.l3a_subnet = ip;
    l3route.l3a_ip_mask = mask;
    l3route.l3a_vrf = vrf;
    rv = bcm_l3_route_delete(unit, &l3route);
    if(rv) {
        printf("Error, bcm_l3_route_delete\n");
        return rv;
    }

    return rv;
}


int dc_enable_raw_data(int unit, int out_port) {

    int i;
    int rv = 0;
    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent;
    uint64 hit_value, hit_mask;
    uint32 hitval[2] = { 0x1234, 0x4321 };
    uint32 zero = 0x0;
    uint32 hitmask = 0xffff;
    int sys_gport;

    presel_id = 0; /* Advanced mode allows same presel ID in different stages */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) { 
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS; 
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageExternal, presel_id); 
        if (BCM_E_NONE != rv) { 
          printf("Error in bcm_field_presel_create_stage_id\n"); 
          return rv; 
        } 
    } else { 
        rv = bcm_field_presel_create(unit, &presel_id); 
        if (BCM_E_NONE != rv) { 
          printf("Error in bcm_field_presel_create\n"); 
          return rv; 
        }
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if(rv) {
        printf("bcm_field_qualify_Stage error\n");
        return rv;
    }

    rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp4DoubleCapacity);
    if(rv) {
        printf("bcm_field_qualify_AppType error\n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_PRESEL_ADD(grp.preselset, presel_id);
    grp.group    = 0;
    grp.priority = 40;
    grp.mode     = bcmFieldGroupModeAuto;
    grp.flags    =
                BCM_FIELD_GROUP_CREATE_WITH_MODE |
                BCM_FIELD_GROUP_CREATE_WITH_ASET |
                BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
                BCM_FIELD_GROUP_CREATE_WITH_ID;

    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionRedirect);

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalHit0);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue0);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalHit2);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue2);

    rv = bcm_field_group_config_create(unit, &grp);
    if(rv) {
        printf("bcm_field_group_config_create error\n");
        return rv;
    }

    for (i = 0; i < 2; i++) {
        /* add 2 entries for each 2 result ports */

        /* port0 */
        rv = bcm_field_entry_create(unit, grp.group, &ent);
        if(rv) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
        }

        rv = bcm_field_qualify_ExternalHit0(unit, ent, 1, 1);
        if(rv) {
            printf("bcm_field_qualify_ExternalHit0 error\n");
            return rv;
        }

        COMPILER_64_ZERO(hit_value);
        COMPILER_64_ZERO(hit_mask);
        COMPILER_64_SET(hit_value, zero, hitval[i]);
        COMPILER_64_SET(hit_mask, zero, hitmask);
        rv = bcm_field_qualify_ExternalValue0(unit, ent, hit_value, hit_mask);
        if(rv) {
            printf("bcm_field_qualify_ExternalValue0 error\n");
            return rv;
        }

        BCM_GPORT_LOCAL_SET(sys_gport, out_port);
        rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, sys_gport);
        if(rv) {
            printf("Error in bcm_field_action_add\n");
            return rv;
        }

        rv = bcm_field_entry_install(unit, ent);
        if(rv) {
            printf("bcm_field_entry_install error\n");
            return rv;
        }

        /* port2 */
        rv = bcm_field_entry_create(unit, grp.group, &ent);
        if(rv) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
        }

        rv = bcm_field_qualify_ExternalHit2(unit, ent, 1, 1);
        if(rv) {
            printf("bcm_field_qualify_ExternalHit0 error\n");
            return rv;
        }

        COMPILER_64_ZERO(hit_value);
        COMPILER_64_ZERO(hit_mask);
        COMPILER_64_SET(hit_value, zero, hitval[i]);
        COMPILER_64_SET(hit_mask, zero, hitmask);
        rv = bcm_field_qualify_ExternalValue2(unit, ent, hit_value, hit_mask);
        if(rv) {
            printf("bcm_field_qualify_ExternalValue0 error\n");
            return rv;
        }

        BCM_GPORT_LOCAL_SET(sys_gport, out_port);
        rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, sys_gport);
        if(rv) {
            printf("Error in bcm_field_action_add\n");
            return rv;
        }

        rv = bcm_field_entry_install(unit, ent);
        if(rv) {
            printf("bcm_field_entry_install error\n");
            return rv;
        }
    }

    return 0;
}

int ipv4_kbp_public_private_semantic(int unit){

    int i = 0;
    int rv = BCM_E_NONE;

    uint32 fec_host = 0x20001000;
    uint32 fec_route = 0x20001001;

    uint32 encap_id = 0x40001000;

    uint32 host  = 0x0a000000;
    uint32 route = 0x0b000000;

    bcm_l3_host_t  l3host;
    bcm_l3_route_t l3route;

    /* verify bcm_l3_host_add/find/delete are not supported to add/get/delete
          public entry to the LEM when public KBP enabled */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags |= BCM_L3_HOST_LOCAL;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = 0;      /* Public host */
    l3host.l3a_intf = fec_host;
    l3host.encap_id = encap_id;

    /* verify bcm_l3_route_add/get/delete for public entry */
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = 0;    /* Public route */
    l3route.l3a_intf = fec_route;

    printf("entries add - start\n");

    for (i = 1; i <= 16; i++)
    {
        rv = bcm_l3_host_add(unit, &l3host);
        if (rv != BCM_E_PARAM) {
             printf("bcm_l3_host_add should return error. rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_add(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_add Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

        l3host.l3a_ip_addr++;
        l3route.l3a_subnet+=0x100;
    }
    printf("entries add - end\n");

    printf("iterator all entries afer add \n");
    bshell(unit, "diag pp kbp iterator ");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags |= BCM_L3_HOST_LOCAL;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = 0;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = 0;

    printf("entries get - start\n");
    for (i = 1; i <= 16; i++)
    {
        rv = bcm_l3_host_find(unit, &l3host);
        if (rv != BCM_E_PARAM) {
             printf("bcm_l3_host_find should return error. rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_get(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_get Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }
        if (l3route.l3a_intf != fec_route) {
             printf("Error, route: fec Failed, expected: 0x%08x, actual: 0x%08x\nentry %d, l3a_subnet=0x%x\n",fec_route,l3route.l3a_intf, i, l3route.l3a_subnet);
             return rv;
        }
        printf("Host - Entry %d: ip = 0x%08x, vrf = %d, fec=0x%08x, encap_id=0x%08x\n",i,l3host.l3a_ip_addr,l3host.l3a_vrf,l3host.l3a_intf,l3host.encap_id);
        printf("Route- Entry %d: ip = 0x%08x, mask=0x%08x, vrf = %d, fec=0x%08x\n",i,l3route.l3a_subnet,l3route.l3a_ip_mask,l3route.l3a_vrf,l3route.l3a_intf);
        l3host.l3a_ip_addr++;
        l3route.l3a_subnet+=0x100;
        l3host.l3a_intf = 0;
        l3host.encap_id = 0;
        l3route.l3a_intf = 0;
    }
    printf("entries get - end\n");

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags |= BCM_L3_HOST_LOCAL;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = 0;

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = route;
    l3route.l3a_ip_mask = 0xffffff00;
    l3route.l3a_vrf = 0;

    printf("entries delete - start\n");
    for (i = 1; i <= 16; i++)
    {
        rv = bcm_l3_host_delete(unit, &l3host);
        if (rv != BCM_E_PARAM) {
             printf("bcm_l3_host_delete should return error. rv=0x%x\n", rv);
             return rv;
        }

        rv = bcm_l3_route_delete(unit, &l3route);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_route_delete Failed, entry %d, l3a_subnet=0x%x, rv=0x%x\n", i, l3route.l3a_subnet, rv);
             return rv;
        }

        l3host.l3a_ip_addr++;
        l3route.l3a_subnet+=0x100;
    }
    printf("entries delete - end\n");

    printf("iterator all entries afer delete \n");
    bshell(unit, "diag pp kbp iterator ");

    return 0;
}
