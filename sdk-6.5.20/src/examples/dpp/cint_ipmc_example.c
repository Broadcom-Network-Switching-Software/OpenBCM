/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: IPMC~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_ipmc_example.c
 * Purpose: Example shows how to configure IP multicast for IPv4 and IPv6.
 *
 * Default Settings:
 * There are two examples, one for IPv4 and one IPv6. Both are similar, except the IP structures.
 *
 * The main example configures the following:
 *  *   Create an IP MC Group - create_ip_mc_group (unit, flags, ipmc_index, dest_port_id, vlan)
 *  *   Add an IP MC Entry - add_ip4mc_entry (unit, mc_ip, src_ip, vlan, mc_id)
 *  Note that the bcm_multicast_t that was created in the create_ip_mc_group() function is saved to pass it
 *  to the add_ip4mc_entry() as an argument. Passing the MC ID alone without the encapsulation
 *  results in an error. Also, trying to create the entry before the group results in an error.
 *  *   Use bcm_ipmc_find to get the entry that was just added.
 *  *   You can also use the individual functions that are included.
 *
 * IPv4 Application: ipv4_mc_example(unit)
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |  | DA  |SA || TIPD1 | Prio | VID 1 ||   DA IPV4   |   SA IPV4   ||  Data |  |
 *  |  |AB:1D| 1 || 0x8100|      |       ||224.224.224.1| 192.128.1.1 ||       |  |
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                     |
 *  |                     | Figure 20: IPv4OEthernet Packet |                     |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Packet will be sent out to port 2
 *
 * Send L3 multicast traffic
 * Configuration:
 *   Destination IP address (MC): 224.224.224.1
 *   Src IP address: 192.128.1.1
 *   Destination mac: 00:00:00:00:ab:1d
 *   Source port is port 1
 * Expected output:
 *   Traffic on port 2
 *
 * IPv6 Application: ipv6_mc_example(unit)
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |  | DA  |SA||TIPD1 |Prio|VID 1||      DA IPV6      |       SA IPV6     ||Data |  |
 *  |  |AB:1D| 1||0x8100|    |     ||FFFF.FFFF.FFFF.FFFF|12DC.0000.0000.0001||     |  |
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |                       +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                       |
 *  |                       | Figure 21: IPv6OEthernet Packet |                       |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Packet will be sent out to port 2.
 *
 * When using the functions individually, please note the order of running,
 * and the returned value in the create_ip_mc_group() function.
 *
 * how to run:
cint examples/dpp/utility/cint_utils_l3.c 
cint examples/dpp/cint_ip_route.c
cint examples/dpp/cint_ipmc_example.c
 */

int print_level = 1;


struct ip_mc_s {
    int      is_ingress_mc; /* indicate if we're running the test using ingress MC or egress MC */
};


ip_mc_s g_ip_mc = 
{
/* is ingress mc */
0
}; 

/*
 * create l3 interface - ingress
 * create_l3_intf
 * defubeed ub cint_ip_route.c
 */

/* Add an IPv4 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int add_ip4mc_entry(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, bcm_multicast_t mc_id){
	bcm_error_t rv = BCM_E_NONE;

	/* Creates the entry */
    /*use add_ip4mc_prefix_entry with full sip and dip mask*/
	rv = add_ip4mc_prefix_entry(unit, mc_ip, src_ip, vlan, mc_id, 0xFFFFFFFF, 0xFFFFFFFF);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}

	return rv;
}


/* Add an IPv4 MC entry with an LPM s_ip_mask
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int add_ip4mc_prefix_entry(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, bcm_multicast_t mc_id, bcm_ip_t s_ip_mask, bcm_ip_t mc_ip_mask){
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    int flags =  BCM_IPMC_SOURCE_PORT_NOCHECK;


    if (lpm_raw_data_enabled) {
        flags |= BCM_IPMC_RAW_ENTRY;
    }
    /* Init the ipmc_addr structure and fill the required fields. */
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = src_ip;
    data.vid = vlan;
    data.flags = flags;
    if (lpm_raw_data_enabled) {
        data.l3a_intf = lpm_payload;
        data.vrf = 1;
    } else {
        data.group = mc_id;
    } 
    data.s_ip_mask = s_ip_mask;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
        return rv;
    }

    return rv;
}

/* Add an IPv4 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
 */
int find_ip4mc_entry(int unit, uint32 vrf, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, uint8 clear_hit_bit){
	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;
	int flags =  BCM_IPMC_SOURCE_PORT_NOCHECK |
						BCM_IPMC_REPLACE;

    if (clear_hit_bit) {
        flags |= BCM_IPMC_HIT_CLEAR;
    }
    printf("key: vrf = %d,vlan = %d,mc_ip = %08x,src_ip = %08x\n", vrf, vlan, mc_ip, src_ip);

	/* Init the ipmc_addr structure and fill the requiered fields. */
	bcm_ipmc_addr_t_init(&data);
    if (vrf != 0) {
        data.rp_id = 0;
    }
    data.vrf = vrf;
	data.mc_ip_addr = mc_ip;
	data.s_ip_addr = src_ip;
	data.vid = vlan;
	data.flags = flags;

	/* Creates the entry */
	rv = bcm_ipmc_find(unit,&data);
    if (rv == BCM_E_NOT_FOUND) {
        printf("Error, not found\n");
        return rv;
    }
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}

    if (data.group) {
      printf("data.group %d\n", data.group);
    } else {
      printf("data.l3a_intf 0x%08x\n", data.l3a_intf);
    }

    if (data.flags & BCM_IPMC_HIT) {
        printf("Entry was hit \n");
    }
    else{
        printf("No hit on entry\n");
    }


    return rv;
}

/* Delete an IPMC entry
*  This function will remove the entry that match the given paramters
   */
int remove_ip4mc_entry(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan){
	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;

	bcm_ipmc_addr_t_init(&data);
	data.mc_ip_addr = mc_ip;
	data.s_ip_addr = src_ip;
	data.vid = vlan;
	data.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;

	rv = bcm_ipmc_remove(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_remove, mc_ip $mc_ip src_ip $src_ip vlan $vlan \n");
		return rv;
	}
	return rv;
}

/* Creates an IP MC Group
*  Here we create the actual MC group.
*  We must run this function before creating the IP MC entry
*  The creation of IPv4 and IPv6 MC Groups is identical
*
*  MC group can be ingress/egress/both, depending on the flags
*  flags: BCM_MULTICAST_INGRESS_GROUP and/or BCM_MULTICAST_EGRESS_GROUP
*/
bcm_multicast_t create_ip_mc_group(int unit, int flags, int ipmc_index, int dest_local_port_id, int nof_ports, int vlan_base, uint8 change_vlan){

    bcm_error_t rv = BCM_E_NONE;
	bcm_cosq_gport_info_t gport_info;
	bcm_cosq_gport_type_t gport_type;
	bcm_gport_t dest_gport;
    bcm_if_t cud;
	gport_type = bcmCosqGportTypeLocalPort;
    int port_indx;
	bcm_multicast_t mc_id = ipmc_index;
	/* The TYPE_L3 Flag will make this group an IPMC group */
    flags |= BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;
    int cud_vlan;

	/* Create the group */
    rv = bcm_multicast_destroy(unit, mc_id);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, in multicast_destroy, mc_id $mc_id\n");
        return rv;
    }

    rv = bcm_multicast_create(unit, flags, &mc_id);
    if (rv != BCM_E_NONE) {
	printf("Error, in multicast_create, mc_id $mc_id flags $flags\n");
	return rv;
    }

	/* Now we will add a destination to the created group */
    for(port_indx = 0; port_indx < nof_ports; ++port_indx) {

      BCM_GPORT_LOCAL_SET(dest_gport,dest_local_port_id);

      cud_vlan = vlan_base + ((change_vlan && port_indx > 1) ? port_indx : 0);
      rv = bcm_multicast_l3_encap_get(unit, mc_id, dest_gport+port_indx, cud_vlan,&cud);
      if (rv != BCM_E_NONE) {
            printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
            return rv;
      }

      if (flags & BCM_MULTICAST_EGRESS_GROUP) {
          rv = bcm_multicast_egress_add(unit,mc_id,dest_gport+port_indx,cud);
          if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_egress_add, ipmc_index $ipmc_index dest_gport $dest_gport \n");
            return rv;
          }
      }

      if (flags & BCM_MULTICAST_INGRESS_GROUP) {
          rv = bcm_multicast_ingress_add(unit,mc_id,dest_gport+port_indx,cud);
          if (rv != BCM_E_NONE) {
              printf("Error, in bcm_multicast_egress_add, ipmc_index $ipmc_index dest_gport $dest_gport \n");
              return rv;
          }
      }
    }

    if(print_level >= 2) {
        printf("create MC group: 0x%08x\n", mc_id);
    }

	return mc_id;
}

/* Add an IPv6 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int add_ip6mc_entry(int unit, bcm_ip6_t mc_ip, bcm_ip6_t src_ip, int vlan, bcm_multicast_t mc_id, int vrf){
	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;
	int i = 0;
	int flags = BCM_IPMC_IP6 | BCM_IPMC_SOURCE_PORT_NOCHECK;

    if (lpm_raw_data_enabled) {
        flags |= BCM_IPMC_RAW_ENTRY;
    }
	bcm_ipmc_addr_t_init(&data);
	for (i = 0; i < 16; i++) {
		data.s_ip6_addr[i] = src_ip[i];
		data.mc_ip6_addr[i] = mc_ip[i];
        data.mc_ip6_mask[i] = 0xFF;
	}

	data.flags = flags;
	data.vid = vlan;
    if (lpm_raw_data_enabled) {
        data.vrf = 1; 
        data.l3a_intf = lpm_payload;
    } else {
        data.vrf = vrf; 
        data.group = mc_id;
    }
	

	rv = bcm_ipmc_add(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}

	return rv;
}

/* Find an IPv6 MC entry
*/
int find_ip6mc_entry(int unit, bcm_ip6_t mc_ip, bcm_ip6_t src_ip, int vlan, bcm_multicast_t mc_id){
	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;
	int i = 0;
	int flags = BCM_IPMC_IP6 | BCM_IPMC_SOURCE_PORT_NOCHECK;

	bcm_ipmc_addr_t_init(&data);
	for (i = 0; i < 16; i++) {
		data.s_ip6_addr[i] = src_ip[i];
		data.mc_ip6_addr[i] = mc_ip[i];
        data.mc_ip6_mask[i] = 0xFF;
	}

	data.flags = flags;
	data.vid = vlan;
	data.group = mc_id;

	rv = bcm_ipmc_find(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc remove, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}

	return rv;
}

/* use raw data payload in KAPs LPM */
int lpm_raw_data_enabled =0;
/* raw payload in Kaps*/
int lpm_payload = 0x12345;
/* 
 * In this example, we send a packet with MC IP 224.224.224.1, dedicated mymac and vlan 2, through the iport.
 * 
 * We then expect the packet to multicast and exit on four different rifs: 
 *      - oport + outrif 2 - will fallback to bridge and be filtered because of same interface filter
 *      - (oport + 1) + outrif 3 (vlan 3)
 *      - (oport + 2) + ourtif 4 (vlan 4)
 *      - (oport + 3) + ourtif 5 (vlan 5)
 *      Expected DA for these packets is the mac_l3_ingress: 00:00:00:00:ab:1d
 */
int run_with_ports_ipv4_mc_example(int unit, int iport, int oport){
    bcm_error_t rv = BCM_E_NONE;
    int ingress_intf;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
    int vlan = 2;
    int ipmc_index = 6000;
    int dest_local_port_id = iport;
    int source_local_port_id = oport;
    int nof_dest_ports = 4, port_ndx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

    /* redirect the packet in the pmf acording to the raw payload recived in the kaps*/
    if (lpm_raw_data_enabled) {
        rv = kaps_raw_data_redirect(unit); 
        if (rv != BCM_E_NONE) {
            printf("Error, in function kaps_raw_data_redirect, \n");
            return rv;
        }
    }

    /* Create the IP MC Group */
    bcm_multicast_destroy(unit,ipmc_index);
    bcm_multicast_t mc_id = create_ip_mc_group(unit, 
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP),
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, TRUE);

    /* Add member VLAN to each port in the multicast group */
    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        rv = bcm_vlan_gport_add(unit, vlan + port_ndx, dest_local_port_id+port_ndx, 0);
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
    
    for(port_ndx = 0; port_ndx < nof_dest_ports; ++port_ndx) {
        
        create_l3_intf_s intf;
        intf.vsi = vlan + port_ndx;
        intf.my_global_mac = mac_l3_ingress;
        intf.my_lsb_mac = mac_l3_ingress;
        intf.mtu_valid = 1;
        intf.mtu = 0;
        intf.mtu_forwarding = 0;
		
		if (lpm_raw_data_enabled){
		    intf.vrf = 1;
            intf.vrf_valid = 1;
		}
     
        rv = l3__intf_rif__create(unit, &intf);
        ingress_intf = intf.rif;        
        if (rv != BCM_E_NONE) {
        	printf("Error, l3__intf_rif__create\n");
        }

        /* Create the IP MC entry */
        rv = add_ip4mc_entry(unit, mc_ip, src_ip, vlan + port_ndx, mc_id);
        if (rv != BCM_E_NONE) return rv;
    }
   /* Example of IPMC find
    *  Here we retrieve data from the IPMC table
    *  For example the IPMC group index
    */
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
   data.mc_ip_addr = mc_ip;
   data.mc_ip_mask = 0xFFFFFFFF;
    data.s_ip_addr = src_ip;
    data.vid = vlan;
    data.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
    /*
    rv = bcm_ipmc_find(0,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, in ipmc_find, mc_ip $mc_ip src_ip $src_ip vlan $vlan \n");
        return rv;
    }*/
    /* data now holds the info of the IPMC group.
     * for example:
     * printf("ipmc index: %d \n",data.ipmc_index);
     */

    return rv;
}


/* Main IPv4 MC Example */
int ipv4_mc_example(int unit){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	/* Set parameters: User can set different parameters.*/
	bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
	bcm_ip_t src_ip = 0xC0800101; /* 192.128.1.1 */
	int vlan = 1;
	int ipmc_index = 6000;
	int dest_local_port_id = 13;
	int source_local_port_id = 13;
    int nof_dest_ports = 4;
	bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

	/* Create the IP MC Group */
	bcm_multicast_destroy(unit,ipmc_index);
	bcm_multicast_t mc_id = create_ip_mc_group(unit, 
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP), 
                                               ipmc_index, dest_local_port_id, nof_dest_ports, vlan, FALSE);

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
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

	/* Create the IP MC entry */
	rv = add_ip4mc_entry(unit, mc_ip, src_ip, vlan, mc_id);
	if (rv != BCM_E_NONE) return rv;

   /* Example of IPMC find
	*  Here we retrieve data from the IPMC table
	*  For example the IPMC group index
	*/
	bcm_ipmc_addr_t data;
	bcm_ipmc_addr_t_init(&data);
	data.mc_ip_addr = mc_ip;
	data.s_ip_addr = src_ip;
	data.vid = vlan;
	data.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
	/*
	rv = bcm_ipmc_find(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_find, mc_ip $mc_ip src_ip $src_ip vlan $vlan \n");
		return rv;
	}*/
	/* data now holds the info of the IPMC group.
	 * for example:
	 * printf("ipmc index: %d \n",data.ipmc_index);
        */

	return rv;
}

/* Example to show how to mask field of the IP-MC key
 * the IPmc key consist of <SIP, Group, RIF (vlan).
 * each of the above fields can be masked.
 *
 * when packet performs IPMC lookup, best match entries is
 * retrieved.
 *
 * The priority of IPMC entries is (high to low):
 *  <SIP,G,RIF>
 *  <SIP,G,x>
 *  <x,G,RIF>
 *  <x,G,*>
 *  <x,x,RIF>
 */

int ipv4_mc_best_match_example(int unit, int in_port, int out_port){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	bcm_ip_t mc_ip = 0xE0E0E004; /* 224.224.224.4 */
	bcm_ip_t src_ip = 0xC0800104; /* 192.128.1.4 */
	int in_vlan = 1; /* incomming vlan for match routing rule*/
    int in_vlan_2 = 2; /* incomming vlan used for mismatch*/
    int out_vlan_start = 1;
    int out_vlan_step = 1; /* by how to increment that out-vlan for each rule */
    int out_vlan; /* aux variable*/
    /* MC groups */
    int nof_groups = 5; /* has to be <= 5 for below example */
    int ipmc_index_s_g_r = 6000;
    int ipmc_index_s_g_x = 6001;
    int ipmc_index_x_g_r = 6002;
    int ipmc_index_x_g_x = 6003;
    int ipmc_index_x_x_r = 6004;
    bcm_multicast_t ipmc_indexes[5] = {0};
    create_l3_intf_s intf;

    /* dest port */
    int dest_local_port_id = out_port;
	int source_local_port_id = in_port;
    int out_port_step = 1; /* whether to increment dest-port in each multicast */
    int nof_dest_ports_per_group = 1; /* number of dest ports per goup*/

    int group_index=0;
	bcm_mac_t my_mac = {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}; /* MY-MAC to be SA of routed packet*/
    int my_mac_inc = 1; /* whether to increment MAC address or not for each interface */

  	/* Create ingress intf, for ethernet termination over vlan 1 (match) */
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, in_vlan);
      return rv;
    }
    
    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* Create ingress intf, for ethernet termination for vlan 2 (mismatch). */
    rv = vlan__open_vlan_per_mc(unit, in_vlan_2, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan_2, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan_2, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, in_vlan_2);
      return rv;
    }
    
    intf.vsi = in_vlan_2;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* create egress interfaces to send through */
    out_vlan = out_vlan_start;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* create interface for out-vlans */
        if(out_vlan != in_vlan && out_vlan != in_vlan_2) {
		    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
		    if (rv != BCM_E_NONE) {
		    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
		    }
		    rv = bcm_vlan_gport_add(unit, out_vlan, source_local_port_id, 0);
		    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, out_vlan);
		      return rv;
		    }
		    
		    intf.vsi = out_vlan;
		    
		    rv = l3__intf_rif__create(unit, &intf);
		    ingress_intf = intf.rif;        
		    if (rv != BCM_E_NONE) {
		    	printf("Error, l3__intf_rif__create\n");
		    }
        }
        if(out_vlan_step == 0) {
            break;
        }
        /* update out-vlan */
        out_vlan += out_vlan_step;
        /* update mac */
        if(my_mac_inc) {
            mac_inc(my_mac);
        }
    }
    if(verbose >= 2) {
        printf("created interfaces \n\r");
    }


    /* fill array for further usage */
    group_index = 0;
    ipmc_indexes[group_index++] = ipmc_index_s_g_r;
    ipmc_indexes[group_index++] = ipmc_index_s_g_x;
    ipmc_indexes[group_index++] = ipmc_index_x_g_r;
    ipmc_indexes[group_index++] = ipmc_index_x_g_x;
    ipmc_indexes[group_index++] = ipmc_index_x_x_r;

    out_vlan = out_vlan_start;
    dest_local_port_id = out_port;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* Create the IP MC Group */
        bcm_multicast_destroy(unit,ipmc_indexes[group_index]);
        ipmc_indexes[group_index] = create_ip_mc_group(unit, 
                                                       (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP), 
                                                       ipmc_indexes[group_index], dest_local_port_id, nof_dest_ports_per_group, out_vlan, FALSE);
        /* increment out-vlan and port */
        out_vlan += out_vlan_step;
        dest_local_port_id += out_port_step;
    }

    if(verbose >= 2) {
        printf("created MC groups\n\r");
    }

	/* Create the IP MC entry with <SIP,G,RIF> */
	rv = add_ip4mc_entry(unit, mc_ip, src_ip, in_vlan, ipmc_indexes[0]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<SIP,G,RIF> %d \n",ipmc_index_s_g_r);
        return rv;
    }

    /* Create the IP MC entry with <SIP,G,*> */
	rv = add_ip4mc_entry(unit, mc_ip, src_ip, 0, ipmc_indexes[1]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<SIP,G,*> %d \n",ipmc_index_s_g_x);
        return rv;
    }

    /* Create the IP MC entry with <*,G,RIF> */
	rv = add_ip4mc_entry(unit, mc_ip, 0, in_vlan, ipmc_indexes[2]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<*,G,RIF> %d \n",ipmc_index_x_g_r);
        return rv;
    }

    /* Create the IP MC entry with <*,G,*> */
	rv = add_ip4mc_entry(unit, mc_ip, 0, 0, ipmc_indexes[3]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<*,G,*> %d \n",ipmc_index_x_g_x);
        return rv;
    }

    /* Create the IP MC entry with <*,*,RIF> */
	rv = add_ip4mc_entry(unit, 0, 0, in_vlan, ipmc_indexes[4]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<*,*,RIF> %d \n",ipmc_index_x_x_r);
        return rv;
    }

    if(verbose >= 2) {
        printf("added MC entries \n\r");
    }

   /* Example of IPMC find
	*  Here we retrieve data from the IPMC table
	*  For example the IPMC group index
	*/
	bcm_ipmc_addr_t data;
	bcm_ipmc_addr_t_init(&data);
	data.mc_ip_addr = mc_ip;
	data.s_ip_addr = src_ip;
	data.vid = in_vlan;
	data.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
	/*
	rv = bcm_ipmc_find(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_find, mc_ip $mc_ip src_ip $src_ip vlan $vlan \n");
		return rv;
	}*/
	/* data now holds the info of the IPMC group.
	 * for example:
	 * printf("ipmc index: %d \n",data.ipmc_index);
        */

	return rv;
}



/*
 * demonstrate handling of MC-IPv4oEth packets, which can be
 * 1. packet to be routed to <RIF, SIP, DIP>, (if IPMC enabled on RIF)
 * 2. packet to be bridged according to <FID, DA>
 * 3. packet to bridged according to <FID, DIP>
 * selected between 2 and 3 is according to bcmSwitchL3McastL2.

 how to run:
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_l2_mact.c
cint ../../../../src/examples/dpp/cint_mact.c
cint ../../../../src/examples/dpp/cint_ipmc_example.c
cint
ipv4_mc_comp_bridge(unit,13,13);
exit;
m IPT_FORCE_LOCAL_OR_FABRIC  FORCE_LOCAL=1

*/

int ipv4_mc_comp_bridge(int unit, int in_port, int out_port){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	bcm_ip_t mc_ip = 0xE0E0E000; /* 224.224.224.0 */
	bcm_ip_t src_ip = 0xC0800100; /* 192.128.1.0 */
	int in_vlan = 1; /* incomming vlan with routing enable */
    int in_vlan_2 = 2; /* incomming vlan with routing disabled */
    int out_vlan_start = 1;
    int out_vlan_step = 1; /* by how to increment that out-vlan for each rule */
    int out_vlan; /* aux variable*/
    /* MC groups */
    int nof_groups = 3; /* has to be <= 3 for below example */
    int ipmc_index_routing = 6000;
    int ipmc_index_dip_bridging = 6001;
    int ipmc_index_da_bridging = 6002;
    bcm_l3_ingress_t l3_ing_if;
    bcm_multicast_t ipmc_indexes[3] = {0};

    /* dest port */
    int dest_local_port_id = out_port;
	int source_local_port_id = in_port;
    int out_port_step = 1; /* whether to increment dest-port in each multicast */
    int nof_dest_ports_per_group = 1; /* number of dest ports per goup*/
    int group_index=0;
	bcm_mac_t my_mac = {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}; /* MY-MAC to be SA of routed packet*/
    int my_mac_inc = 1; /* whether to increment MAC address or not for each interface */
    bcm_mac_t dest_mac = {0x01, 0x00, 0x5e, 0x60, 0xe0, 0x02}; /* MY-MAC to be SA of routed packet*/

    create_l3_intf_s intf;

  	/* Create ingress intf, for ethernet termination over vlan-1 with routing enabled */
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, in_vlan);
      return rv;
    }
    
    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    /* Create ingress intf, for ethernet termination for vlan-2 with routing disabled. */
    rv = vlan__open_vlan_per_mc(unit, in_vlan_2, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan_2, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan_2, source_local_port_id, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, in_vlan_2);
      return rv;
    }
    
    intf.vsi = in_vlan_2;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*  disable IPMC routing on interface */
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    /* one control for IPv4 and IPv6*/
    l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST|BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    rv = bcm_l3_ingress_create(unit, l3_ing_if, &ingress_intf);
    if (rv != BCM_E_NONE) {
      printf("failed in bcm_l3_ingress_create %d \n\r", rv);
      return rv;
    }
    if(verbose >= 2) {
        printf("disable IPv4-MC routing on intf %d\n\r",ingress_intf);
    }

    /* create egress interfaces to send through */
    out_vlan = out_vlan_start;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* create interface for out-vlans */
        if(out_vlan != in_vlan && out_vlan != in_vlan_2) {
		    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
		    if (rv != BCM_E_NONE) {
		    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
		    }
		    rv = bcm_vlan_gport_add(unit, out_vlan, source_local_port_id, 0);
		    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		    	printf("fail add port(0x%08x) to vlan(%d)\n", source_local_port_id, out_vlan);
		      return rv;
		    }
		    
		    intf.vsi = out_vlan;
		    
		    rv = l3__intf_rif__create(unit, &intf);
		    ingress_intf = intf.rif;        
		    if (rv != BCM_E_NONE) {
		    	printf("Error, l3__intf_rif__create\n");
		    }
        }
        if(out_vlan_step == 0) {
            break;
        }
        /* update out-vlan */
        out_vlan += out_vlan_step;
        /* update mac */
        if(my_mac_inc) {
            mac_inc(my_mac);
        }
    }
    if(verbose >= 2) {
        printf("created out-interfaces \n\r");
    }


    /* fill array for further usage */
    group_index = 0;
    ipmc_indexes[group_index++] = ipmc_index_routing;
    ipmc_indexes[group_index++] = ipmc_index_dip_bridging;
    ipmc_indexes[group_index++] = ipmc_index_da_bridging;

    out_vlan = out_vlan_start;
    dest_local_port_id = out_port;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* Create the IP MC Group */
        bcm_multicast_destroy(unit,ipmc_indexes[group_index]);
        ipmc_indexes[group_index] = create_ip_mc_group(unit, 
                                                       (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP), 
                                                       ipmc_indexes[group_index], dest_local_port_id, nof_dest_ports_per_group, out_vlan, FALSE);
        /* increment out-vlan and port */
        out_vlan += out_vlan_step;
        dest_local_port_id += out_port_step;
    }

    if(verbose >= 2) {
        printf("created MC groups\n\r");
    }

    /* Create the IP MC entry with <SIP,G,RIF>
      packet is to be forwarded according to this entry if
    - IPMC routing is enabled on intf
       */
	rv = add_ip4mc_entry(unit, mc_ip, src_ip, in_vlan, ipmc_indexes[0]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<SIP,G,RIF> %d \n",ipmc_indexes[0]);
        return rv;
    }

    /*find_ip4mc_entry(unit, mc_ip, src_ip, in_vlan,0);
    remove_ip4mc_entry(unit, mc_ip, src_ip, in_vlan);
    remove_ip4mc_entry(unit, mc_ip, src_ip, in_vlan); */

    /* Create the IP MC entry with <FID,DIP>
       packet is to be forwarded according to this entry if
       - IPMC routing is disabled on intf
       - bcm_switch_control_set(unit,bcmSwitchL3McastL2,1);
     */
	rv = add_ip4mc_entry(unit, mc_ip, 0, in_vlan_2, ipmc_indexes[1]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<FID,DIP> %d \n",ipmc_indexes[1]);
        return rv;
    }

    /*
    find_ip4mc_entry(unit, mc_ip, 0, in_vlan_2,0);
    remove_ip4mc_entry(unit, mc_ip, 0, in_vlan_2);
    remove_ip4mc_entry(unit, mc_ip, 0, in_vlan_2);*/

    /* find and remove */


    /* Create the IP MC entry with <FID,MC-DA>
        packet is to be forwarded according to this entry if
        - IPMC routing is disabled on intf
        - bcm_switch_control_set(unit,bcmSwitchL3McastL2,0);
       */
    rv = l2_entry_add(unit,dest_mac,in_vlan_2,1,0,ipmc_indexes[2]);
    if (rv != BCM_E_NONE) {
        printf("fail add L2:<FID,DA> %d \n",ipmc_indexes[2]);
        return rv;
    }

	return rv;
}

/* Main IPv6 MC Example
*  This example is very similar to the IPv4 example.
*/
int ipv6_mc_example(int unit){
	/* src: 12CD:0000:0000:0000:0000:0000:0000:0001 */
	/* dst: FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF */
	/* Destination port is 2*/
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	/* Set parameters: User can set different parameters. */
	bcm_ip6_t s_ip6 = {18, 205, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
	bcm_ip6_t mc_ip6 = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
	int ipmc_index = 6001;
	int dest_local_port_id = 2;
	int vlan = 1;
	int source_local_port_id = 1;
	bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

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
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

	/* Create the IPMC group, same as we did for IPv6	 */
	bcm_multicast_t mc_id = create_ip_mc_group(unit, 
                                               (g_ip_mc.is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP), 
                                               ipmc_index, dest_local_port_id, vlan, FALSE);

	/* Create the IPv6 MC Entry */
	rv = add_ip6mc_entry(unit, mc_ip6, s_ip6, vlan, mc_id);
    if (rv != BCM_E_NONE) return rv;

	return rv;
}

/* Print IPv4 MC entry */
int ipv4_mc_print(bcm_ipmc_addr_t data)
{
    if (data.group) {
        printf("G=0x%08x S=0x%08x V=0x%08x -->  MC_GROUP=0x%08x\n", data.mc_ip_addr, data.s_ip_addr, data.vid, data.group);
    } else {
        printf("G=0x%08x S=0x%08x V=0x%08x -->  L3INTF=0x%08x\n", data.mc_ip_addr, data.s_ip_addr, data.vid, data.l3a_intf);
    }

	return BCM_E_NONE;
}

/* IPMC table traverse and counter for all entries */
int ipv4_mc_traverse_callback(int uint, bcm_ipmc_addr_t* data, int* ud)
{
    ipv4_mc_print(*data);
    ++*ud;
	return BCM_E_NONE;
}


/* A example how to use bcm_ipmc_traverse() to get all IPv4 MC entries */
int ipv4_mc_traverse(int unit)
{
    int rv = BCM_E_NONE;
    int ipv4_mc_counter = 0;
    rv = bcm_ipmc_traverse(unit, 0, ipv4_mc_traverse_callback, &ipv4_mc_counter);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_traverse returned %s failed\n", bcm_errmsg(rv));
        return rv;
    }
    printf("IPv4 MC entries:%d\n", ipv4_mc_counter);
}

