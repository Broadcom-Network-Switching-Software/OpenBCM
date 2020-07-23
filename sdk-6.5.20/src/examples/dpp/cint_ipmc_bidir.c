/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_ipmc_bidir.c
 * Purpose: Example shows how to configure IP multicast BIDIR.
 *
 * Soc-Property:
 *    - ipmc_pim_mode: MC routing mode (bitmap).
 *          - value: has to include 0x2:PIM-BIDIR, i.e. bit(1) has to be set.
 *    - ipmc_pim_bidir_check_db: to select DB to perform mapping from group to as RPID
 *          - values:
 *              -	0: Exact-match
 *              -	1: TCAM for ranges (not supported yet)
 *              -	2: both (not supported yet)
 *
 * Example configuration:
 *  - create two RP-IDs (rpid1, rpid2)
 *  - create two ingress intf (vlan1, vlan2)
 *  - define admission rules
 *      - vlan1 with rpid1
 *      - vlan2 with rpid2
 *  - create VRF
 *  - map two interfaces to VRF
 *  - open 4 MC-groups (to be used as forwarding destination)
 *  - handle 3 MC groups
 *     - G0=224.224.224.0 mapped to RPID-1, and MC-group-0
 *     - G1=224.224.224.1 mapped to RPID-2, and MC-group-1
 *     - G2=224.224.224.2 not mapped to RPID, means treated as PIM MC, forwarded to MC-group-2
 *
 * packet and tests:
 *  - G0 from intf1 ==> forwarded to MC-group0.
 *  - G0 from intf2 ==> filtered due to admission test.
 *  - G1 from intf1 ==> filtered due to admission test.
 *  - G1 from intf2 ==> forwarded to MC-group1
 *  - G2 from intf1 ==> forwarded to MC-group2 (not BIDIR traffic)
 *  - G2 from intf2 ==> forwarded to MC-group3 (not BIDIR traffic)
 * how to run:
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c 
cint ../../../../src/examples/dpp/cint_ipmc_example.c 
cint ../../../../src/examples/dpp/cint_mact.c 
cint ../../../../src/examples/dpp/cint_ipmc_bidir.c 
cint ../../../../src/examples/dpp/cint_field_ipmc_bidir.c 
cint 
ipv4_mc_bidir_example(unit,13,14); 
 
 
Remarks: 
ARAD: 
- cint_field_ipmc_bidir: used to drop packet that fail admission test. 
- admission test take place in LEM. 
- mapping to RP-ID take place in SEM/TCAM
 
API usage: 
- bcm_ipmc_add has to usages depends on data.rp_id, 
  
  1. if rp_id != BCM_IPMC_RP_ID_INVALID, BIDIR usage, and then API add into two DBs 
        a. RP identification i.e. mapping <VRF,G> to RP-ID in SEM
        b. forwarding i.e. mapping <VRF,G> to Group" in LEM
     same call to bcm_ipmc_add does both insertion 
 
 2. otherwise (rp_id = BCM_IPMC_RP_ID_INVALID) PIM usage (non-BIDIR DB), and then used to add entry to forwarding DB according to <RIF,SIP,G>
 - for remove/get if data.rp_id != BCM_IPMC_RP_ID_INVALID then BIDIR DBs are considered. 
  otherwise  PIM DB is considered
     
*/ 



int add_ipmc_bidir_group_add(int unit, int vrf, bcm_ip_t mc_ip, int rp_id,int mc_group){
    	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;
	int flags =  BCM_IPMC_SOURCE_PORT_NOCHECK;


	/* Init the ipmc_addr structure and fill the requiered fields. */
	bcm_ipmc_addr_t_init(&data);
	data.mc_ip_addr = mc_ip;
	data.vrf = vrf;
    data.rp_id = rp_id;
    data.group = mc_group;
	/* data.group = mc_id;*/

	/* Creates the entry */
	rv = bcm_ipmc_add(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}
    if(verbose >= 1) {
        printf("add BIDIR entry <vrf = 0x%08x, G = 0x%08x> --> <RP-ID = 0x%08x, Group: 0x%08x\n\r", vrf, mc_ip,rp_id,mc_group);
    }


	return rv;
}


/* 
 * example function.
 */

int ipv4_mc_bidir_example(int unit, int in_port, int out_port){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf[2];
    int egress_intf;
	int in_vlan = 1; /* incomming vlan for match routing rule*/
    int in_vlan_2 = 2; /* incomming vlan used for mismatch*/
    int vrf = 10; /* VRF incomming interfaces mapped to */
    int out_vlan_start = 100;
    int out_vlan_step = 1; /* by how to increment that out-vlan for each rule */
    int out_vlan; /* aux variable*/
    /* MC groups */
    int nof_groups = 4; /* has to be <= 5 for below example */
    int ip_groups[3] = {0xE0E0E000,0xE0E0E001,0xE0E0E002}; /* 224.224.224.0 - 224.224.224.2*/
    bcm_multicast_t mc_groups[4] = {6000,6001,6002,6003};

    /* dest port */
    int dest_local_port_id = out_port;
	int source_local_port_id = in_port;
    int out_port_step = 1; /* whether to increment dest-port in each multicast */
    int nof_dest_ports_per_group = 1; /* number of dest ports per goup*/
    
    int group_index=0;
	bcm_mac_t my_mac = {0x00, 0x0C, 0x00, 0x02, 0x00, 0x00}; /* MY-MAC to be SA of routed packet*/
    int my_mac_inc = 1; /* whether to increment MAC address or not for each interface */
    int rp_id1 = 255;
    int rp_id2 = 10;
    uint8 logic_test = 0;
	int sip =  0xc0800107; /* 192.128.1.7 */

    create_l3_intf_s intf;

    /* init PMF to drop upon admission test fail
       note this doesn't add any entry in TCAM PMF,
       it's just rule that says if packet is BIDIR and addmision lookup fail in LEM then drop packet
     */
    ipmc_bidir_setup(unit,0,0);

  	/* Create ingress intf, over vlan 1 */
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
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf[0] = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* Create ingress intf over vlan 2 */
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
    ingress_intf[1] = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    if(verbose >= 1) {
        printf("created egress interfaces\n\r");
    }
    /* create egress interfaces to send packet out through */
    out_vlan = out_vlan_start;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* create interface for out-vlans */
	    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
	    if (rv != BCM_E_NONE) {
	    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
	    }
	    rv = bcm_vlan_gport_add(unit, out_vlan, dest_local_port_id, 0);
	    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
	    	printf("fail add port(0x%08x) to vlan(%d)\n", dest_local_port_id, out_vlan);
	      return rv;
	    }
	    
	    intf.vsi = out_vlan;
		intf.my_global_mac = my_mac;
		intf.my_lsb_mac = my_mac;
	    
	    rv = l3__intf_rif__create(unit, &intf);
	    egress_intf = intf.rif;        
	    if (rv != BCM_E_NONE) {
	    	printf("Error, l3__intf_rif__create\n");
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

    /* create rendezvous point 1 */
    rv = bcm_ipmc_rp_create(unit,BCM_IPMC_RP_WITH_ID,&rp_id1);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_create %d\n",rp_id1);
        return rv;
    }

    /* create rendezvous point 2 */
    rv = bcm_ipmc_rp_create(unit,BCM_IPMC_RP_WITH_ID,&rp_id2);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_create %d\n",rp_id2);
        return rv;
    }

    /* open Multicast group */
    group_index = 0;
    out_vlan = out_vlan_start;
    dest_local_port_id = out_port;
    for(group_index = 0; group_index < nof_groups; ++group_index) {
        /* Create the IP MC Group */
        bcm_multicast_destroy(unit,mc_groups[group_index]);
        mc_groups[group_index] = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, mc_groups[group_index], dest_local_port_id, nof_dest_ports_per_group, out_vlan, FALSE);
        /* increment out-vlan and port */
        out_vlan += out_vlan_step;
        dest_local_port_id += out_port_step;
    }

    /* set admission test, thus interface 2 success with RP-ID , interface 2 will fail in admission test so packet will be dropped*/
    rv = bcm_ipmc_rp_add(unit,rp_id1,ingress_intf[0]);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_add intf: %08x\n",ingress_intf[0]);
        return rv;
    }

    rv = bcm_ipmc_rp_add(unit,rp_id2,ingress_intf[1]);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_add intf: %08x\n",ingress_intf[1]);
        return rv;
    }

    /* handle IPMC BIDIR group: map it to RP-ID and forwarding to MC-group */
    rv = add_ipmc_bidir_group_add(unit, vrf, ip_groups[0], rp_id1, mc_groups[0]);
    if (rv != BCM_E_NONE) {
        printf("Error add_ipmc_bidir_group_add ip_group: 0x%08x",ip_groups[0]);
        return rv;
    }

    rv = add_ipmc_bidir_group_add(unit, vrf, ip_groups[1], rp_id2, mc_groups[1]);
    if (rv != BCM_E_NONE) {
        printf("Error add_ipmc_bidir_group_add ip_group: 0x%08x",ip_groups[1]);
        return rv;
    }

    /* Create the not BIDIR entry forward according <*,G,*> */
	rv = add_ip4mc_entry(unit, ip_groups[2], sip, in_vlan, mc_groups[2]);
	if (rv != BCM_E_NONE) {
        printf("fail at :<*,G,*> %d \n",ip_groups[2]);
        return rv;
    }

    rv = add_ip4mc_entry(unit, ip_groups[2], sip, in_vlan_2, mc_groups[3]);
    if (rv != BCM_E_NONE) {
        printf("fail at :<*,G,*> %d \n",ip_groups[3]);
        return rv;
    }

    if(verbose >= 1) {
        printf("added MC entries \n\r");
    }

    /* test get remove operations */
    if (logic_test) {
        find_ip4mc_entry(unit,vrf,ip_groups[0],0,0,0);
        find_ip4mc_entry(unit,vrf,ip_groups[2],0,0,0);
        find_ip4mc_entry(unit,0,ip_groups[0],0,in_vlan,0);
        find_ip4mc_entry(unit,0,ip_groups[2],0,in_vlan,0);
    }

	return rv;
}

/* 
    
    assume running above function 
 */

int ipv4_mc_bidir_rp_get(int unit, int rp_id){
    int intf_max = 20;
    bcm_if_t intf_array[20]; 
    int intf_count;
    int rv;
    int indx;

    rv = bcm_ipmc_rp_get(unit,rp_id,intf_max,intf_array,&intf_count);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_create %d\n",rp_id);
        return rv;
    }

    printf("count: %d\n",intf_count);
    for (indx = 0; indx < intf_count; ++indx) {
        printf("%d\n",intf_array[indx]);
    }

    return rv;
}



int ipv4_mc_bidir_rp_intf_operation(int unit,int rp_id){
    uint32 rv;
    int intf_count;
    bcm_if_t intfs[10];

    rv = bcm_ipmc_rp_create(unit,BCM_IPMC_RP_WITH_ID,&rp_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_create %d\n",rp_id);
        return rv;
    }

    intf_count = 5;
    intfs[0] = 100;
    intfs[1] = 120;
    intfs[2] = 200;
    intfs[3] = 300;
    intfs[4] = 401;
    rv = bcm_ipmc_rp_set(unit,rp_id, intf_count, intfs);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",rp_id);
        return rv;
    }

    
    rv = ipv4_mc_bidir_rp_get(unit,rp_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",rp_id);
        return rv;
    }


    printf("delete  %d\n",intfs[2]);
    rv = bcm_ipmc_rp_delete(unit, rp_id, intfs[2]);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",intfs[2]);
        return rv;
    }

    printf("delete  %d\n again" ,intfs[2]);
    rv = bcm_ipmc_rp_delete(unit, rp_id, intfs[2]);
    print rv;

    rv = ipv4_mc_bidir_rp_get(unit,rp_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",rp_id);
        return rv;
    }

    printf("delete  all\n");
    rv = bcm_ipmc_rp_delete_all(unit, rp_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",rp_id);
        return rv;
    }

    rv = ipv4_mc_bidir_rp_get(unit,rp_id);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_ipmc_rp_set %d\n",rp_id);
        return rv;
    }

    return rv;
}

/*ipv4_mc_bidir_rp_intf_operation(0,120);*/


/* Add an IPv4 MC BIDIR eligible entry and mapping Group to RP-id */
int add_ipmc_bidir_range_add(int unit, int vrf, bcm_ip_t mc_ip, int vrf_mask, bcm_ip_t mc_ip_mask, int rp_id){
    bcm_ipmc_range_t
        range;
    int dummy_id;
	bcm_error_t rv = BCM_E_NONE;

	/* Init the ipmc_addr structure and fill the requiered fields. */
	bcm_ipmc_range_t_init(&range);
    range.mc_ip_addr = mc_ip;
    range.mc_ip_addr_mask = mc_ip_mask; /* no mask */
    range.vrf = vrf;
    range.vrf_mask = vrf_mask; 
    range.flags =  BCM_IPMC_RANGE_PIM_BIDIR; /* has to be set*/
    range.rp_id = rp_id;

	/* Creates the entry */
	rv = bcm_ipmc_range_add(unit,&dummy_id, &range);
	if (rv != BCM_E_NONE) {
		printf("Error, in bcm_ipmc_range_add\n");
		return rv;
	}
	return rv;
}

/*
 * Add LEM entry <vrf, G> --> mc-group
 * - addr: IP Group
 * - vrf: max value depeneds on soc-property in arad.soc
 * - mc_group: Multicast group
 */
int add_ipmc_bidir_frwrd_entry(int unit, int vrf, uint32 group, int mc_group) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_ip_addr = group;
  l3host.l3a_vrf = vrf;
  l3host.l3a_ipmc_ptr = mc_group;

  rc = bcm_l3_host_add(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %x \n", rc);
  }

  if(verbose >= 1) {
      print_host("add entry direct", group,vrf);
      printf("---> MC-group=0x%08x \n", mc_group);
  }

  return rc;
}


