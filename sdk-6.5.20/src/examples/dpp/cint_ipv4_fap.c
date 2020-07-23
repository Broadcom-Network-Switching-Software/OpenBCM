/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: L3 Routing~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_ipv4_fap.c
 * Purpose: Example of how to deliver IP traffic from one source to multiple receivers.
 *
 * CINT assumes diag_init has been executed
 *
 * for one device call bcm_l3_main(int unit, int second_unit, int dst_port) with second_unit<0
 * Example: bcm_l3_main(unit,-1, 1);
 *
 * for two devices when one device configures the second device call bcm_l3_main(unit, 1, 1) 
 * make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1
 *
 * traffic example for two devices:
 *
 * the cint generates 4 routes and hosts listed below
 *   device 0:
 *   ---------
 *     port: 1           host   6737170e : 103.55.23.14
 *     port: 1           route  78400000 : 120.64.0.0         mask  ffc00000 : 255.192.0.0
 *     port: 2           host   6837170f : 104.55.23.15
 *     port: 2           route  88400000 : 136.64.0.0         mask  ffe00000 : 255.224.0.0
 * 
 *   device 1:
 *   ---------
 *     port: 100         host   6537170c : 101.55.23.12
 *     port: 100         route  70000000 : 112.0.0.0          mask  ff000000 : 255.0.0.0
 *     port: 101         host   6637170d : 102.55.23.13
 *     port: 101         route  64000000 : 100.0.0.0          mask  ff800000 : 255.128.0.0
 *
 * L3 one simple path application: 
 * Sets up interfaces and route to match packets send from the CPU 
 *
 * Example: 
 * bcm_l3_test(unit);
 * pkt_test1();
 * 
 * L3 application: 
 * Sample usage and random routes. User responsible to set traffic according to settings. 
 * This can be done by changing pkt test script or by using traffic generator with the 
 * matching packet header.
 * 
 * Example:
 *      BCM> cint cint_qos_l3.c
 *      BCM> cint cint_ipv4_fap.c
 *      BCM> cint
 *      cint> bcm_l3_main(unit, second_unit, dst_port);
 * 
 * Note: You can also use the individual functions that are included in the example file.
 */
 
 
/*
 * Reset CINT 
 * Enable logging
 * Setup global variables
 */
int verbose = 1; 
uint32 seed = 0x00001234;

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};
bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

/*
 * delete l3 interface
 */
int delete_l3_ingress(int unit) {

    int rc;
    
    rc = bcm_l3_intf_delete_all(unit);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_intf_delete_all failed: %d \n", rc);
    }
    
    return rc;
}


/************************************
 * Utilities
 */
uint32 sal_rand(void) {
    seed = seed + 0x4321; 
    return (seed);
}

void random_host(uint32 *host)
{
    *host =  ((sal_rand()%192)<<24)+((sal_rand()%192)<<16)+((sal_rand()%192)<<8)+(sal_rand()%192);
}

void print_host(char *type, uint32 host)
{
    int a,b,c,d;
    
    a = (host >> 24) & 0xff;
    b = (host >> 16) & 0xff;
    c = (host >> 8 ) & 0xff;
    d = host & 0xff;
    printf("%s  %x : %d.%d.%d.%d \n", type, host, a,b,c,d);
}

/*
 * Add Route
 */
int add_route(int unit, uint32 addr, uint32 mask, int vrf, int intf) {
    int rc;
    bcm_l3_route_t l3rt;
    
    bcm_l3_route_t_init(&l3rt);
    
    l3rt.l3a_flags = BCM_L3_RPF; 

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    
    rc = bcm_l3_route_add(unit, &l3rt);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %d \n", rc);
    }
    return rc;
}

/*
 * Add Host
 */
int add_host(int unit, uint32 addr, int vrf, int intf) {
    int rc;
    bcm_l3_host_t l3host;
    
    bcm_l3_host_t_init(&l3host);
    
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;
    l3host.l3a_modid = 0;
    l3host.l3a_port_tgid = 0; 

    
    rc = bcm_l3_host_add(unit, l3host);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %d \n", rc);
    }
    return rc;
}

/* delete functions */
int delete_host(int unit, int intf) {
    int rc;
    bcm_l3_host_t l3host;
    
    bcm_l3_host_t_init(&l3host);
    
    l3host.l3a_flags = 0;
    l3host.l3a_intf = intf;
    
    rc = bcm_l3_host_delete_all(unit, l3host);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %d \n", rc);
    }
    return rc;
}

/* 
 * DUNE UI commands to dump/verfiy
 *  -   soc_ppd_api frwrd_ipv4 vrf_route_get vrf_ndx 3 ip_address 001.002.003.000 prefix_len 20 
 *  -   soc_ppd_api frwrd_ipv4 host_get ip_address 010.011.012.1 vrf_ndx 0 
 *
 * L3 IPv4 table commands
 *  -   soc_ppd_api frwrd_ipv4 host_get_block  dumps host table
 *  -   soc_ppd_api frwrd_ipv4 uc_route_get_block type type_fast  dumps uc route table
 *  -   soc_ppd_api frwrd_ipv4 vrf_route_get_block vrf_ndx N type type_fast  dumps vrf route table for vrf index N
 *
 * Diagnostic Shell L3 commands
 *  -   l3 host add vrf=1 ip=2.2.2.2 fec=9
 *  -   l3 route add vrf=5 ip=3.3.3.0 mask=255.255.254.0 fec=5 
 */

int MAX_PORTS=4;
int MAX_HOSTS=100;
int MAX_ROUTES=100;
int MAX_VRF=8;

/*
 * Sample usage and multiple routes. 
 * You responsible to set traffic according to settings.
 */
int bcm_l3_main(int unit, int second_unit, int dst_port) {
    int CINT_NO_FLAGS = 0;
    int rc;
    int i;  
    int egress_port;
    int vlan = 17;
    int tmp_port;
    int vrf = 3;
    
    int ingress_intf[MAX_PORTS];
    int egress_intf[MAX_PORTS];
    int encap_id[MAX_PORTS];
    
    int egress_port_list[MAX_PORTS] = {100,101,1,2};
    int egress_port_list_one_device[MAX_PORTS] = {dst_port,dst_port,dst_port,dst_port};
    
    
	bcm_port_config_t c;
    bcm_pbmp_t p,u;
    
    if (second_unit<0)
    {
        
        for (i=0; i<MAX_PORTS; i++) { 
            egress_port=egress_port_list_one_device[i];
        
		    bcm_port_config_get(unit, &c);
		    BCM_PBMP_ASSIGN(p, c.e);
		    rc = bcm_vlan_create(unit, vlan);
		    if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
		    	printf ("bcm_vlan_create failed: %d \n", rc);
		    	return rc;
		    }
		    rc = bcm_vlan_port_add(unit, vlan, p, u);
		    	if (rc != BCM_E_NONE) {
		    	return rc;
		    }
		    
		    create_l3_intf_s intf;
		    intf.vsi = vlan;
		    intf.my_global_mac = mac_l3_ingress;
		    intf.my_lsb_mac = mac_l3_ingress;
		    intf.vrf_valid = 1;
		    intf.vrf = vrf;
		    intf.rpf_valid = 1;
		    intf.flags |= BCM_L3_RPF;
		    intf.urpf_mode = bcmL3IngressUrpfLoose;
		    rc = l3__intf_rif__create(unit, &intf);
		    ingress_intf[i] = intf.rif;
		    if (rc != BCM_E_NONE) {
		    	printf("Error, l3__intf_rif__create");
		    }
		        
            create_l3_egress_s l3_eg;
            sal_memcpy(l3_eg.next_hop_mac_addr, mac_l3_egress, 6);
            l3_eg.out_tunnel_or_rif = ingress_intf[i]; 
            l3_eg.allocation_flags = CINT_NO_FLAGS;
            l3_eg.vlan   = vlan;
            l3_eg.out_gport   = egress_port;
            l3_eg.arp_encap_id = encap_id[i];
            l3_eg.fec_id = egress_intf[i]; 
            
            rc = l3__egress__create(unit,&l3_eg);
            if (rc != BCM_E_NONE) {
            printf ("l3__egress__create failed: %d \n", rc);
            }
            
            encap_id[i] = l3_eg.arp_encap_id;
            egress_intf[i] = l3_eg.fec_id;   
        }
    }
    else
    {  
        
        for (i=0; i<MAX_PORTS; i++) {    
            egress_port=egress_port_list[i];

			rc = bcm_vlan_create(unit, vlan);
			if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
				printf ("bcm_vlan_create failed: %d \n", rc);
				return rc;
			}

			bcm_port_config_get(unit, &c);
			BCM_PBMP_ASSIGN(p, c.e);
			rc = bcm_vlan_port_add(unit, vlan, p, u);
				if (rc != BCM_E_NONE) {
				return rc;
			}

		    create_l3_intf_s intf;
			intf.vsi = vlan;
			intf.my_global_mac = mac_l3_ingress;
			intf.my_lsb_mac = mac_l3_ingress;
			intf.vrf_valid = 1;
			intf.vrf = vrf;
			ntf.rpf_valid = 1;
			ntf.flags |= BCM_L3_RPF;
			ntf.urpf_mode = bcmL3IngressUrpfLoose;
            
            create_l3_egress_s l3_eg;
            sal_memcpy(l3_eg.next_hop_mac_addr, mac_l3_egress, 6);
            l3_eg.allocation_flags = CINT_NO_FLAGS;
            l3_eg.vlan   = vlan;
            l3_eg.out_gport   = egress_port;
            l3_eg.arp_encap_id = encap_id[i];
            l3_eg.fec_id = egress_intf[i]; 

            if (i<(MAX_PORTS/2)) /*ingress on dev 0 egress on dev 1*/
            {
			    rc = l3__intf_rif__create(unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    rc = l3__intf_rif__create(second_unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
    		    l3_eg.out_tunnel_or_rif = ingress_intf[i];
                rc = l3__egress__create(second_unit, &l3_eg);
                if (rc != BCM_E_NONE) {
                  printf ("l3__egress__create failed: %d \n", rc);
                  return rc;
                }
                l3_eg.allocation_flags |= BCM_L3_WITH_ID;
                rc = l3__egress__create(unit, &l3_eg);          
                if (rc != BCM_E_NONE) {
                  printf ("l3__egress__create failed: %d \n", rc);
                  return rc;
                }
            }
            else /*egress on dev 0 ingress on dev 1*/
            {
			    rc = l3__intf_rif__create(second_unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    rc = l3__intf_rif__create(unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    l3_eg.out_tunnel_or_rif = ingress_intf[i];
                rc = l3__egress__create(unit, &l3_eg);
                if (rc != BCM_E_NONE) {
                  printf ("l3__egress__create failed: %d \n", rc);
                  return rc;
                }
                l3_eg.allocation_flags |= BCM_L3_WITH_ID;
                rc = l3__egress__create(second_unit, &l3_eg);          
                if (rc != BCM_E_NONE) {
                  printf ("l3__egress__create failed: %d \n", rc);
                  return rc;
                }
            }
			encap_id[i] = l3_eg.arp_encap_id;
			egress_intf[i] = l3_eg.fec_id;
        }
    }
    
    
    uint32 host[5];
    uint32 route[5];
    uint32 mask;
    int prefix=1;
    
    host[0] = ((101%192)<<24)+((55%192)<<16)+((23%192)<<8)+(12%192); 
    host[1] = ((102%192)<<24)+((55%192)<<16)+((23%192)<<8)+(13%192);
    host[2] = ((103%192)<<24)+((55%192)<<16)+((23%192)<<8)+(14%192);
    host[3] = ((104%192)<<24)+((55%192)<<16)+((23%192)<<8)+(15%192);
    host[4] = ((105%192)<<24)+((55%192)<<16)+((23%192)<<8)+(16%192);
    
    route[0] = ((112%192)<<24)+((52%192)<<16)+((22%192)<<8)+(1%192);
    route[1] = ((100%192)<<24)+((53%192)<<16)+((22%192)<<8)+(1%192);
    route[2] = ((120%192)<<24)+((85%192)<<16)+((22%192)<<8)+(1%192);
    route[3] = ((136%192)<<24)+((95%192)<<16)+((22%192)<<8)+(1%192);
    route[4] = ((32%192)<<24)+((60%192)<<16)+((22%192)<<8)+(1%192);
    
    
    for (i=0; i<4; i++) {
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS]; 
        } 
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }    
        
        if (verbose) {
            printf("on egress interface (fec): %d      port: %d           ", egress_intf[i%MAX_PORTS], tmp_port);
            print_host("host", host[i]);
        }
        add_host (unit, host[i], vrf, egress_intf[i%MAX_PORTS]);
        if (second_unit >= 0)
        {
            rc = add_host (second_unit, host[i], vrf, egress_intf[i%MAX_PORTS]);
            if (rc != BCM_E_NONE) {
              printf("Error. add_host failed: $d\n", rc);
              return BCM_E_FAIL;
            }    
        }
    }
    
    for (i=0; i<4; i++) { 
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS]; 
        } 
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }  
        prefix--;
        if (prefix == 0) {
            prefix = 24;
        }    
        mask = 0xffffffff << prefix;
        route[i] = route[i] & mask;
        rc = add_route(unit, route[i], 0xfffff000, vrf, egress_intf[i%MAX_PORTS]);
        if (rc != BCM_E_NONE) {
          printf("Error. add_route failed: $d\n", rc);
          return BCM_E_FAIL;
        }    
        if (second_unit >= 0)
        {
            rc = add_route(second_unit, route[i], 0xfffff000, vrf, egress_intf[i%MAX_PORTS]);
            if (rc != BCM_E_NONE) {
              printf("Error. add_route failed: $d\n", rc);
              return BCM_E_FAIL;
            }    
        }    
        if (verbose) {
            printf("on egress interface (fec): %d      port: %d     ", egress_intf[i%MAX_PORTS], tmp_port);
            print_host("route", route[i]);
            printf("                                                ");
            print_host("mask", mask);
        }
    }
    
	return 0;
}

int route_traverse_scale(int unit, int index, bcm_l3_route_t *info, void *user_data)
{
    if (index > 8) {
        printf ("route_traverse_scale, index out of range.\n");
        traverse_rc = BCM_E_FAIL;
        return traverse_rc;
    }
    if (info->l3a_intf != l3rts_traverse[index].l3a_intf) {
        printf ("route_traverse_scale, l3a_intf: Expected 0x%x, entry 0x%x\n", l3rts_traverse[index].l3a_intf, info->l3a_intf);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_ip_mask != l3rts_traverse[index].l3a_ip_mask) {
        printf ("route_traverse_scale, l3a_ip_mask: Expected 0x%x, entry 0x%x\n", l3rts_traverse[index].l3a_ip_mask, info->l3a_ip_mask);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_subnet != l3rts_traverse[index].l3a_subnet) { 
        printf ("route_traverse_scale, l3a_subnet: Expected 0x%x, entry 0x%x\n", l3rts_traverse[index].l3a_subnet, info->l3a_subnet);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_vrf != l3rts_traverse[index].l3a_vrf) {
        printf ("route_traverse_scale, l3a_vrf: Expected %d, entry %d\n", l3rts_traverse[index].l3a_vrf, info->l3a_vrf);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_flags2 != l3rts_traverse[index].l3a_flags2) {
        printf ("route_traverse_scale, l3a_flags2: Expected 0x%x, entry 0x%x\n", l3rts_traverse[index].l3a_flags2, info->l3a_flags2);
        traverse_rc = BCM_E_FAIL;
    }

    traverse_route_counter++;

    printf ("Route traverse index: %d verified\n", index);
    return 0;
}

int host_traverse_scale(int unit, int index, bcm_l3_host_t *info, void *user_data)
{
    if (index > 2) {
        printf ("host_traverse_scale, index out of range.\n");
        traverse_rc = BCM_E_FAIL;
        return traverse_rc;
    }

    if (info->l3a_intf != l3hosts_traverse[index].l3a_intf) {
        printf ("host_traverse_scale, l3a_intf: Expected 0x%x, entry 0x%x\n", l3hosts_traverse[index].l3a_intf, info->l3a_intf);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_ip_addr != l3hosts_traverse[index].l3a_ip_addr) {
        printf ("host_traverse_scale, l3a_ip_addr: Expected 0x%x, entry 0x%x\n", l3hosts_traverse[index].l3a_ip_addr, info->l3a_ip_addr);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_vrf != l3hosts_traverse[index].l3a_vrf) {
        printf ("host_traverse_scale, l3a_vrf: Expected %d, entry %d\n", l3hosts_traverse[index].l3a_vrf, info->l3a_vrf);
        traverse_rc = BCM_E_FAIL;
    }

    if (info->l3a_flags2 != l3hosts_traverse[index].l3a_flags2) {
        printf ("host_traverse_scale, l3a_flags2: Expected 0x%x, entry 0x%x\n", l3hosts_traverse[index].l3a_flags2, info->l3a_flags2);
        traverse_rc = BCM_E_FAIL;
    }

    traverse_host_counter++;

    printf ("Host traverse index: %d verified.\n", index);
    return 0;
}

/*
 *  The purpose of this cint is to create:
 *  9 interfaces with different VRFs and MACs
 *  11 hosts/routes with the same IPV4 DIP and VRF, but different prefix length.
 *      Prefix length 32 goes to LEM host, prefix_length*4 goes to LEM scale route, in addition to KAPS
 *      the remaining entries with prefix > prefix_length*4 go only to KAPS
 */
int NOF_INTERFACES = 9;
bcm_l3_host_t  l3hosts_traverse[2];
bcm_l3_route_t l3rts_traverse[9];
int traverse_rc = 0;
int traverse_route_counter = 0;
int traverse_host_counter = 0;

int bcm_l3_ipv4_uc_fib_scale(int unit, int port0, int port1, int port2, int port3, int prefix_length) {

    int rc,i;
    bcm_l3_intf_t l3if;
    bcm_if_t l3egid;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x00};

    int vlan[NOF_INTERFACES];/*Each interface VLAN and VRF */
    uint8 mac_last_byte[NOF_INTERFACES];/*All interfaces has the same my MAC/dest MAC differ only in the last byte */
    int egress_intf[NOF_INTERFACES];
    int ports[NOF_INTERFACES];

    /*Set each interface parameters*/
    vlan[0] = 2000;
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_INTERFACES; i++) {
        vlan[i] = vlan[i-1]+10;
        mac_last_byte[i] = (mac_last_byte[i-1]+0x20) % 0x100;
    }

    ports[0] = port0;
    ports[1] = port1;
    ports[2] = port2;
    ports[3] = port3;

    for (i=0; i < NOF_INTERFACES; i++) {
        /*Create L3 interface*/
        bcm_l3_intf_t_init(l3if);
        if(rc == BCM_E_EXISTS) {
          printf ("vlan %d aleady exist \n", vlan[i]);
        }
        mac_l3_ingress_add[5] = mac_last_byte[i];
        sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress_add, 6);
        l3if.l3a_vid = vlan[i];
        l3if.l3a_vrf = vlan[i];
        rc = bcm_l3_intf_create(unit, &l3if);
        if (rc != BCM_E_NONE) {
            return rc;
        }

        /*Create L3 egress*/
        create_l3_egress_s l3eg;
        l3eg.out_tunnel_or_rif = l3if.l3a_intf_id;
        mac_l3_engress_add[5] = mac_last_byte[i];
        sal_memcpy(l3eg.next_hop_mac_addr, mac_l3_engress_add, 6);
        l3eg.vlan   = vlan[i];
        l3eg.out_gport   = ports[i % 4];
        

        rc = l3__egress__create(unit, &l3eg);
        if (rc != BCM_E_NONE) {
          printf ("l3__egress__create failed \n");
          return rc;
        }

        egress_intf[i] = l3eg.fec_id;
    }

    bcm_ip_t ip = 0x12345678;

    /*Create Host*/
    bcm_l3_host_t_init(&l3hosts_traverse[0]);
    l3hosts_traverse[0].l3a_ip_addr = ip;
    l3hosts_traverse[0].l3a_vrf = vlan[0];
    /* dest is FEC + OutLif */
    l3hosts_traverse[0].l3a_intf = egress_intf[0]; /* fec */
    rc = bcm_l3_host_add(unit, &l3hosts_traverse[0]);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_add failed: %d \n", rc);
      return rc;
    }

    /* An additional KAPS entry at the same prefix_length as the prefix_length in the LEM, points towards egress_intf[1] */
    bcm_l3_route_t_init(&l3rts_traverse[0]);
    l3rts_traverse[0].l3a_subnet = ip;
    l3rts_traverse[0].l3a_ip_mask = bcm_ip_mask_create(32);
    l3rts_traverse[0].l3a_vrf = vlan[0];
    l3rts_traverse[0].l3a_intf = egress_intf[1]; /* fec */;
    l3rts_traverse[0].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
    rc = bcm_l3_route_add(unit, l3rts_traverse[0]);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed: %d \n", rc);
      return rc;
    }

    /*Create route*/
    for (i=1; i < NOF_INTERFACES; i++) {
        if ((8 - i) == prefix_length) {
            /*Create Host*/
            bcm_l3_host_t_init(&l3hosts_traverse[1]);
            l3hosts_traverse[1].l3a_ip_addr = ip & bcm_ip_mask_create(32 - i * 4);
            l3hosts_traverse[1].l3a_vrf = vlan[0];
            l3hosts_traverse[1].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
            l3hosts_traverse[1].l3a_intf = egress_intf[i]; /* fec */
            rc = bcm_l3_host_add(unit, &l3hosts_traverse[1]);
            if (rc != BCM_E_NONE) {
              printf ("bcm_l3_host_add failed: %d \n", rc);
              return rc;
            }

            /* An additional KAPS entry at the same prefix_length as the prefix_length in the LEM, points towards egress_intf[0] */
            bcm_l3_route_t_init(&l3rts_traverse[i]);
            l3rts_traverse[i].l3a_subnet = ip & bcm_ip_mask_create(32 - i * 4);
            l3rts_traverse[i].l3a_ip_mask = bcm_ip_mask_create(32 - i * 4);
            l3rts_traverse[i].l3a_vrf = vlan[0];
            l3rts_traverse[i].l3a_intf = egress_intf[i+1]; /* fec */;
            l3rts_traverse[i].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
            rc = bcm_l3_route_add(unit, l3rts_traverse[i]);
            if (rc != BCM_E_NONE) {
              printf ("bcm_l3_route_add failed: %d \n", rc);
              return rc;
            }
        } else {
            bcm_l3_route_t_init(&l3rts_traverse[i]);
            l3rts_traverse[i].l3a_subnet = ip & bcm_ip_mask_create(32 - i * 4);
            l3rts_traverse[i].l3a_ip_mask = bcm_ip_mask_create(32 - i * 4);
            l3rts_traverse[i].l3a_vrf = vlan[0];
            l3rts_traverse[i].l3a_intf = egress_intf[i]; /* fec */;
            l3rts_traverse[i].l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
            rc = bcm_l3_route_add(unit, l3rts_traverse[i]);
            if (rc != BCM_E_NONE) {
              printf ("bcm_l3_route_add failed: %d \n", rc);
              return rc;
            }
        }
    }

    rc = bcm_l3_route_traverse(unit, 0x0, 0, 0, route_traverse_scale, NULL);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_traverse failed: %d \n", rc);
      return rc;
    }

    if ((traverse_rc != BCM_E_NONE) || (traverse_route_counter != 9)) {
      printf ("bcm_l3_route_traverse failed: %d \n", rc);
      return traverse_rc;
    }

    rc = bcm_l3_host_traverse(unit, 0x0, 0, 0, host_traverse_scale, NULL);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_traverse failed: %d \n", rc);
      return rc;
    }

    if ((traverse_rc != BCM_E_NONE) || (traverse_host_counter != 2)) {
      printf ("bcm_l3_host_traverse failed: %d \n", rc);
      return traverse_rc;
    }

    return rc;
}

